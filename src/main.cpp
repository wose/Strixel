#include "AlgSimple.h"
#include "Preview.h"
#include "Solution.h"

#include "imgui.h"
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
                sum += 255 - (uchar)**line;
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
            image.at<uchar>(line.pos()) = image.at<uchar>(line.pos()) + penalty > 255
                ? 255 : image.at<uchar>(line.pos()) + penalty;
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

    Solution solution;
    AlgSimple simple;
    Preview preview{image};
    Preview preview2{image};

    while (!glfwWindowShouldClose(window)) {
        if(!glfwGetWindowAttrib(window, GLFW_ICONIFIED) && glfwGetWindowAttrib(window, GLFW_VISIBLE)) {
            glfwPollEvents();
            ImGui_ImplGlfwGL3_NewFrame();

            // Settings
            ImGui::BeginMainMenuBar();
            int menuHeight = ImGui::GetTextLineHeightWithSpacing();
            if(ImGui::BeginMenu("File")) {
                if(ImGui::MenuItem("Quit")) {
                    glfwSetWindowShouldClose(window, GL_TRUE);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();

            bool open = false;
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
            ImGui::SetNextWindowPos(ImVec2(0, menuHeight));
            ImGui::SetNextWindowSize(ImVec2(400, ImGui::GetIO().DisplaySize.y - menuHeight));
            if (ImGui::Begin("###main", &open, ImVec2(0, 0), 0.5f,
                             ImGuiWindowFlags_NoTitleBar |
                             ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
                             ImGuiWindowFlags_NoSavedSettings))
            {
                if (ImGui::TreeNodeEx("Solution", ImGuiTreeNodeFlags_DefaultOpen)) {
                    solution.draw();
                    ImGui::TreePop();
                }
                if (ImGui::TreeNodeEx("Algorithm", ImGuiTreeNodeFlags_DefaultOpen)) {
                    static int currentAlgorithm = 0;
                    ImGui::Combo("###AlgorithmCombo", &currentAlgorithm, "simple\0empty\0\0");
                    simple.draw();
                    ImGui::TreePop();
                }
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                if (ImGui::Button("Calculate Pattern", ImVec2(ImGui::GetContentRegionAvailWidth(), 20))) {
                    // start
                }
            }
            ImGui::End();
            ImGui::PopStyleVar();

            if (ImGui::Begin("Settings")) {
                ImGui::SliderFloat("Black Score", &blackScore, 0.1, 10.0);
                ImGui::SliderFloat("White Score", &whiteScore, 0.1, 10.0);
                ImGui::SliderInt("Penalty", &penalty, 0, 255);
                ImGui::SliderInt("Passes", &passes, 1, 5000);
                ImGui::InputInt("Pins", &pins);
                ImGui::InputFloat("Pin Thickness", &pinThickness);
                ImGui::SliderInt("Min Space", &minSpace, 0, pins / 3);

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
            preview.draw(pattern);
            preview2.draw(pattern);

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
