#include "VerticalSlider.hpp"

#include <imgui.h>

#include "VSliderController.hpp"

namespace psm_gui::ui::panels {

void VerticalSlider::draw(AppState& s, const PanelRect& r) {
  controller::VSliderController sliderCtl_(s);
  ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
                           ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
  ImGui::SetNextWindowPos(r.pos);
  ImGui::SetNextWindowSize(r.size);
  ImGui::Begin("##VerticalSlider", nullptr, flags);
  float sliderWidth = r.size.x * 0.15f;
  float sliderHeight = r.size.y * 0.8f;
  float centerX = (r.size.x - sliderWidth) * 0.5f;
  float centerY = (r.size.y - sliderHeight) * 0.5f;

  ImGuiStyle& style = ImGui::GetStyle();
  ImVec4 oldGrabColor = style.Colors[ImGuiCol_SliderGrab];
  ImVec4 oldGrabActiveColor = style.Colors[ImGuiCol_SliderGrabActive];
  float oldGrabRounding = style.GrabRounding;
  float oldFrameRounding = style.FrameRounding;

  ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.3f, 0.7f, 0.9f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_SliderGrabActive,
                        ImVec4(0.4f, 0.8f, 1.0f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 0.8f));
  ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,
                        ImVec4(0.15f, 0.15f, 0.15f, 0.9f));
  ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
  ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 8.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, sliderWidth * 0.8f);

  ImGui::SetCursorPos(ImVec2(centerX, centerY));
  if (ImGui::VSliderInt("##VerticalSlider", ImVec2(sliderWidth, sliderHeight),
                        &s.sliders.vertical_slider, -100, 100, "")) {
    sliderCtl_.updateImage();
  }
  char label[32];
  snprintf(label, sizeof(label), "%d", s.sliders.vertical_slider);
  ImVec2 textSize = ImGui::CalcTextSize(label);
  ImGui::SetCursorPos(ImVec2(centerX + ((sliderWidth - textSize.x) * 0.5f),
                             centerY + sliderHeight + 5.0f));
  ImGui::Text("%s", label);

  // Display the dynamic label showing what this slider does
  ImVec2 labelSize = ImGui::CalcTextSize(s.sliders.vertical_label.c_str());
  ImGui::SetCursorPos(ImVec2(centerX + ((sliderWidth - labelSize.x) * 0.5f),
                             centerY + sliderHeight + 25.0f));
  ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%s", s.sliders.vertical_label.c_str());

  ImGui::PopStyleVar(3);
  ImGui::PopStyleColor(5);

  ImGui::End();
}

}  // namespace psm_gui::ui::panels
