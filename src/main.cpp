#include "AlgSimple.h"
#include "Preview.h"
#include "Solution.h"

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

#include <stdio.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <opencv2/opencv.hpp>

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

    Solution solution(image);
    AlgSimple simple;
    Preview preview{solution};
    Preview preview2{solution};

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
                    simple.calculate(solution);
                }
            }
            ImGui::End();
            ImGui::PopStyleVar();

            // Preview
            preview.draw(solution);
            preview2.draw(solution);

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
