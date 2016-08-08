#include "imgui.h"
#include "Preview.h"

#include <sstream>

int Preview::nextPreviewID_ = 0;

Preview::Preview(const Solution& solution) :
    id_{++nextPreviewID_}
{
    cv::Mat image = solution.getImage();
    glGenTextures(1, &textureID_);
    glBindTexture(GL_TEXTURE_2D, textureID_);

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
}

void Preview::draw(const Solution& solution)
{
    std::ostringstream title;
    title << "Preview##" << id_;
    if(ImGui::Begin(title.str().c_str())) {
        ImGui::Checkbox("Image", &showImage_);
        ImGui::SameLine(100);
        ImGui::Checkbox("Border", &showBorder_);
        ImGui::SameLine(200);
        ImGui::Checkbox("Pins", &showPins_);
        ImGui::SameLine(300);
        ImGui::Checkbox("Strings", &showStrings_);

        ImGui::SliderFloat("String Alpha", &stringAlpha_, 0.0, 1.0);
        ImGui::ColorEditMode(ImGuiColorEditMode_RGB);
        ImGui::ColorEdit3("Background Color", backgroundColor_);
        ImGui::ColorEdit3("String Color", stringColor_);

        ImGui::Separator();
        auto drawList = ImGui::GetWindowDrawList();
        auto start = ImGui::GetCursorScreenPos();

        if (showImage_) {
            ImGui::Image(reinterpret_cast<void*>(textureID_), ImVec2(500, 500));
        } else {
            ImColor bg{backgroundColor_[0], backgroundColor_[1], backgroundColor_[2], backgroundColor_[3]};
            ImGui::Dummy(ImVec2(500, 500));
            drawList->AddCircleFilled(ImVec2{start.x + 250, start.y + 250}, 250, bg, 64);
        }

        if (showBorder_) {
            drawList->AddCircle(ImVec2(start.x + 250, start.y + 250), 250, ImColor(0, 200, 0), 64, 3.0);
        }

        if (showPins_) {
            for(int pin = 0; pin < solution.getPins(); ++pin) {
                ImVec2 center;
                center.x = start.x + 250 + cos(pin * 360.0 / solution.getPins() * M_PI / 180) * 250;
                center.y = start.y + 250 + sin(pin * 360.0 / solution.getPins() * M_PI / 180) * 250;
                drawList->AddCircleFilled(center, pinThickness, ImColor(255, 100, 0), 8);
            }
        }

        if (showStrings_ && !solution.empty()) {
            drawList->PathClear();
            for (auto const& pin : solution) {
                ImVec2 pos;
                pos.x = start.x + 250 + cos(pin * 360.0 / pins * M_PI / 180) * 250;
                pos.y = start.y + 250 + sin(pin * 360.0 / pins * M_PI / 180) * 250;
                drawList->PathLineTo(pos);
            }
            drawList->PathStroke(ImColor(ImVec4{stringColor_[0], stringColor_[1], stringColor_[2], stringAlpha_}), false);
            drawList->PathClear();
        }
    }
    ImGui::End();
}
