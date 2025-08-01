#include "HorizontalSlider.hpp"

#include <imgui.h>

#include "HSliderController.hpp"

namespace psm_gui::ui::panels {

void HorizontalSlider::draw(AppState& s, const PanelRect& r) {
  controller::HSliderController sliderCtl_(s);
  ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
                           ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
  ImGui::SetNextWindowPos(r.pos);
  ImGui::SetNextWindowSize(r.size);
  ImGui::Begin("##HorizontalSlider", nullptr, flags);
  float sliderWidth = r.size.x * 0.8f;
  float sliderHeight = r.size.y * 0.25f;

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
  ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, sliderHeight * 0.8f);

  ImGui::SetCursorPos(ImVec2(centerX, centerY));
  ImGui::PushItemWidth(sliderWidth);
  if (ImGui::SliderInt("##HorizontalSlider", &s.controls.horizontal_slider,
                       -100, 100, "")) {
    sliderCtl_.updateImage();
  }
  ImGui::PopItemWidth();

  char label[32];
  snprintf(label, sizeof(label), "%d", s.controls.horizontal_slider);
  ImVec2 textSize = ImGui::CalcTextSize(label);
  ImGui::SetCursorPos(
      ImVec2((r.size.x - textSize.x) * 0.5f, centerY + sliderHeight + 5.0f));
  ImGui::Text("%s", label);

  // Display the dynamic label showing what this slider does
  ImVec2 labelSize = ImGui::CalcTextSize(s.controls.horizontal_label.c_str());
  ImGui::SetCursorPos(
      ImVec2((r.size.x - labelSize.x) * 0.5f, centerY + sliderHeight + 25.0f));
  ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%s",
                     s.controls.horizontal_label.c_str());

  ImGui::PopStyleVar(3);
  ImGui::PopStyleColor(5);

  ImGui::End();
}

}  // namespace psm_gui::ui::panels
