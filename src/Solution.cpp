#include "imgui.h"
#include "Solution.h"

#include "pugixml.hpp"

Solution::Solution(const cv::Mat image) :
    image_{image}
{
}

void Solution::draw()
{
    ImGui::SliderInt("Passes", &passes_, 1, 5000);
    ImGui::InputInt("Pins", &pins_);
    ImGui::InputFloat("Pin Thickness", &pinThickness_);
    ImGui::Separator();
    if (ImGui::Button("Save")) {
        save("foo.xml");
    }
    ImGui::SameLine();
    if (ImGui::Button("Load")) {
        load("foo.xml");
    }
}

void Solution::save(const char* file)
{
    pugi::xml_document doc;
    auto solution = doc.append_child("Solution");
    solution.append_attribute("Passes") = passes_;
    solution.append_attribute("PinThickness") = pinThickness_;
    solution.append_attribute("Pins") = pins_;

    auto pattern = solution.append_child("Pattern");
    for (auto pin : pattern_) {
        pattern.append_child("Pin").append_attribute("Idx") = pin;
    }

    doc.save_file(file);
}

void Solution::load(const char* file)
{
    pugi::xml_document doc;
    auto result = doc.load_file(file);
    if (!result) {
        throw std::runtime_error(result.description());
    }

    auto solution = doc.child("Solution");

    passes_ = solution.attribute("Passes").as_int();
    pinThickness_ = solution.attribute("PinThickness").as_float();
    pins_ = solution.attribute("Pins").as_int();

    auto pattern = doc.child("Solution").child("Pattern");
    for (const auto& pin : pattern) {
        pattern_.push_back(pin.attribute("Idx").as_int());
    }
}
