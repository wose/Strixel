#include "imgui.h"

#include "AlgSimple.h"
#include "Solution.h"

void AlgSimple::draw()
{
    ImGui::SliderInt("Penalty", &penalty_, 0, 255);
    ImGui::SliderInt("Min Space", &minSpace_, 0, 50);
}

void AlgSimple::calculate(Solution& solution)
{
}
