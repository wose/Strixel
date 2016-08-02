#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"

#include <stdio.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <opencv2/opencv.hpp>

#include <atomic>
#include <mutex>


float blackScore = 1;
float whiteScore = 0.5;
int pins = 200;
int minSpace = 0;
int passes = 1000;
int penalty = 10;
float pinThickness = 1.0;
static std::atomic<bool> calculating{false};
static std::atomic<int> currentPass{0};
std::vector<int> pattern;

void calculatePattern(cv::Mat image)
{
    calculating = true;
    pattern.clear();

    int currentPin= 0;
    pattern.push_back(currentPin);

    for(currentPass = 1; currentPass <= passes; currentPass++)
    {
        std::cout << "Current Pass: " << currentPass;
        float maxScore = 0;
        int nextPin = 0;
        int prevPin = 0;
        cv::Point2d start{image.cols / 2 + cos(currentPin * 360 / pins * M_PI / 180) * image.cols / 2,
                image.rows / 2 + sin(currentPin * 360 / pins * M_PI / 180) * image.rows / 2};

        for (int pin = ((currentPin + 1 + minSpace) % 360); ((pin + minSpace) % 360) != currentPin; pin = ++pin % 360)
        {
            std::cout << " " << pin;
            cv::Point2d end{image.cols / 2 + cos(pin * 360 / pins * M_PI / 180) * image.cols / 2,
                    image.rows / 2 + sin(pin * 360 / pins * M_PI / 180) * image.rows / 2};

            cv::LineIterator line(image, start, end, 8);
            float sum = 0;
            for (int index = 0; index < line.count; ++index, line++)
            {
                sum += pow(255 - (uchar)**line, 2);
            }

            float score = sum / line.count;
            if (score > maxScore)
            {
                maxScore = score;
                nextPin = pin;
            }
        }
        cv::Point2d end{image.cols / 2 + cos(nextPin * 360 / pins * M_PI / 180) * image.cols / 2,
                image.rows / 2 + sin(nextPin * 360 / pins * M_PI / 180) * image.rows / 2};

        cv::LineIterator line(image, start, end, 8);
        for (int index = 0; index < line.count; ++index, line++)
        {
            image.at<uchar>(line.pos()) += penalty;
        }

        prevPin = currentPin;
        currentPin = nextPin;
        pattern.push_back(currentPin);

        std::cout << std::endl;
    }

    calculating = false;
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

int main(int argc, char** argv)
{
    if(argc != 2) {
        std::cout << "Usage:\n\t" << argv[0] << " IMAGE" << std::endl;
        return 1;
    }
    // Setup window
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow* window = glfwCreateWindow(1280, 720, "HBVisionGraphEditor", NULL, NULL);
    glfwMakeContextCurrent(window);
    gl3wInit();

    // Setup ImGui binding
    ImGui_ImplGlfwGL3_Init(window, true);

    ImVec4 clear_color = ImColor(39, 40, 34);

    cv::Mat image = cv::imread(argv[1]);
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 image.cols,
                 image.rows,
                 0,
                 GL_BGR,
                 GL_UNSIGNED_BYTE,
                 image.ptr());

    bool showImage = true;
    bool showBorder = false;
    bool showPins = true;
    bool showStrings = true;
    float stringAlpha = 0.2;

    while (!glfwWindowShouldClose(window)) {
        if(!glfwGetWindowAttrib(window, GLFW_ICONIFIED) && glfwGetWindowAttrib(window, GLFW_VISIBLE)) {
            glfwPollEvents();
            ImGui_ImplGlfwGL3_NewFrame();

            // Settings
            ImGui::BeginMainMenuBar();
            if(ImGui::BeginMenu("File")) {
                if(ImGui::MenuItem("Quit")) {
                    glfwSetWindowShouldClose(window, GL_TRUE);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();

            if(ImGui::Begin("Settings")) {
                ImGui::SliderFloat("Black Score", &blackScore, 0.1, 10.0);
                ImGui::SliderFloat("White Score", &whiteScore, 0.1, 10.0);
                ImGui::SliderInt("Penalty", &penalty, 0, 255);
                ImGui::SliderInt("Passes", &passes, 1, 5000);
                ImGui::InputInt("Pins", &pins);
                ImGui::InputFloat("Pin Thickness", &pinThickness);
                ImGui::SliderInt("Min Space", &minSpace, 0, pins / 3);
                ImGui::SliderFloat("String Alpha", &stringAlpha, 0.0, 1.0);

                ImGui::Separator();

                if(calculating) {
                    ImGui::ProgressBar(0.4);
                    ImGui::SameLine();
                    ImGui::Text("Calculating... %d / %d", currentPass.load(), passes);
                } else {
                    if(ImGui::Button("Calculate")) {
                        cv::Mat grayscale;
                        cv::cvtColor(image, grayscale, cv::COLOR_BGR2GRAY);
                        calculatePattern(grayscale);
                        // start calculating the current pattern
                    }
                }
            }
            ImGui::End();

            // Preview
            if(ImGui::Begin("Preview")) {
                ImGui::Checkbox("Image", &showImage);
                ImGui::SameLine(200);
                ImGui::Checkbox("Border", &showBorder);
                ImGui::Checkbox("Pins", &showPins);
                ImGui::SameLine(200);
                ImGui::Checkbox("Strings", &showStrings);

                ImGui::Separator();
                auto drawList = ImGui::GetWindowDrawList();
                auto start = ImGui::GetCursorScreenPos();

                if(showImage) {
                    ImGui::Image(reinterpret_cast<void*>(texID), ImVec2(500, 500));
                } else {
                    ImGui::Dummy(ImVec2(500, 500));
                    drawList->AddRectFilled(start, ImVec2(start.x + 500, start.y + 500), ImColor(255, 255, 255));
                }

                if(showBorder) {
                    drawList->AddCircle(ImVec2(start.x + 250, start.y + 250), 250, ImColor(0, 200, 0), 64, 3.0);
                }

                if(showPins) {
                    for(int pin = 0; pin < pins; ++pin) {
                        ImVec2 center;
                        center.x = start.x + 250 + cos(pin * 360.0 / pins * M_PI / 180) * 250;
                        center.y = start.y + 250 + sin(pin * 360.0 / pins * M_PI / 180) * 250;
                        drawList->AddCircleFilled(center, pinThickness, ImColor(255, 100, 0), 8);
                    }
                }

                if (showStrings && !pattern.empty()) {
                    drawList->PathClear();
                    for (auto pin : pattern) {
                        ImVec2 pos;
                        pos.x = start.x + 250 + cos(pin * 360.0 / pins * M_PI / 180) * 250;
                        pos.y = start.y + 250 + sin(pin * 360.0 / pins * M_PI / 180) * 250;
                        drawList->PathLineTo(pos);
                    }
                    drawList->PathStroke(ImColor(ImVec4{0, 0, 0, stringAlpha}), false);
                    drawList->PathClear();
                }
            }
            ImGui::End();

            // Rendering
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui::Render();
            glfwSwapBuffers(window);
        } else {
            glfwWaitEvents();
        }
    }

    // Cleanup
    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();

    return 0;
}
