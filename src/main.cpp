#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"

#include <stdio.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <opencv2/opencv.hpp>

#include <atomic>
#include <mutex>


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

    while (!glfwWindowShouldClose(window)) {
        if(!glfwGetWindowAttrib(window, GLFW_ICONIFIED) && glfwGetWindowAttrib(window, GLFW_VISIBLE)) {
            glfwPollEvents();
            ImGui_ImplGlfwGL3_NewFrame();

            // Settings
            float blackScore = 1;
            float whiteScore = 0.5;
            int pins = 200;
            int passes = 1000;
            float pinThickness = 1.0;
            static std::atomic<bool> calculating{false};
            static std::atomic<int> currentPass{0};

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
                ImGui::SliderInt("Passes", &passes, 1, 5000);
                ImGui::InputInt("Pins", &pins);
                ImGui::InputFloat("Pin Thickness", &pinThickness);

                ImGui::Separator();

                if(calculating) {
                    ImGui::ProgressBar(0.4);
                    ImGui::SameLine();
                    ImGui::Text("Calculating... %d / %d", currentPass.load(), passes);
                } else {
                    if(ImGui::Button("Calculate")) {
                        // start calculating the current pattern
                    }
                }
            }
            ImGui::End();

            // Preview
            if(ImGui::Begin("Preview")) {
                static bool showImage = true;
                static bool showBorder = true;
                static bool showPins = true;
                static bool showStrings = true;
                ImGui::Checkbox("Image", &showImage);
                ImGui::SameLine(200);
                ImGui::Checkbox("Border", &showBorder);
                ImGui::Checkbox("Pins", &showPins);
                ImGui::SameLine(200);
                ImGui::Checkbox("Strings", &showStrings);

                ImGui::Separator();
                ImGui::Image(reinterpret_cast<void*>(texID), ImVec2(500, 500));
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
