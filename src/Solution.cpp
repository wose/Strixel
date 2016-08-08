#include "imgui.h"
#include "Solution.h"

Solution::Solution(const cv::Mat image) :
    image_{image}
{
}

void Solution::draw()
{
    ImGui::SliderInt("Passes", &passes_, 1, 5000);
    ImGui::InputInt("Pins", &pins_);
    ImGui::InputFloat("Pin Thickness", &pinThickness_);
}
