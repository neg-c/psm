#include "controls_area.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "style/style_manager.h"
#include "ui/ui_renderer.h"

namespace psm_gui {

ControlsArea::ControlsArea(AppState& state, UIRenderer& uiRenderer)
    : m_state(state), m_uiRenderer(uiRenderer) {}

void ControlsArea::render(float windowWidth, float windowHeight) {
  // Calculate button size based on window size
  const float buttonWidth = windowWidth * 0.15f;
  const float buttonHeight = windowHeight * 0.5f;  // Increase button height

  setupColumns(windowWidth);

  // First column: Load/Convert buttons and combo box
  ImGui::BeginGroup();

  {
    UIStyleManager spacing(ImGuiStyleVar_ItemSpacing,
                           ImVec2(buttonWidth * 0.2f, buttonHeight * 0.2f));
    UIStyleManager align(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));

    renderButtons(buttonWidth, buttonHeight);
    renderConversionCombo(windowWidth * 0.2f);
  }

  ImGui::EndGroup();

  ImGui::NextColumn();

  // Second column: Slider
  renderSlider(ImGui::GetColumnWidth() - ImGui::GetStyle().FramePadding.x * 2);

  ImGui::Columns(1);
}

void ControlsArea::renderVerticalSlider(float width, float height) {
  // Style the vertical slider to match the horizontal one
  UIStyleManager frameBg(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
  UIStyleManager sliderGrab(ImGuiCol_SliderGrab,
                            ImVec4(0.4f, 0.4f, 0.8f, 0.8f));
  UIStyleManager sliderGrabActive(ImGuiCol_SliderGrabActive,
                                  ImVec4(0.5f, 0.5f, 0.9f, 0.8f));
  UIStyleManager grabMinSize(ImGuiStyleVar_GrabMinSize, 10.0f);

  // Center the slider in the available space - use more of the width
  const float sliderWidth = width * 0.8f;  // Increased from 0.6f
  const float sliderPosX = (width - sliderWidth) * 0.5f;

  ImGui::SetCursorPos(ImVec2(sliderPosX, 10.0f));

  ImGui::PushItemWidth(sliderWidth);

  float sliderHeight = height - 20.0f;

  ImGui::VSliderInt("##VerticalSlider", ImVec2(sliderWidth, sliderHeight),
                    &m_state.vertical_slider_value, 0, 100, "%d%%");

  ImGui::PopItemWidth();
}

void ControlsArea::renderButtons(float buttonWidth, float buttonHeight) {
  // Style for button text alignment
  UIStyleManager buttonTextAlign(ImGuiStyleVar_ButtonTextAlign,
                                 ImVec2(0.5f, 0.5f));

  UIStyleManager buttonPadding(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 4.0f));

  float textWidth = ImGui::CalcTextSize("Load").x;
  float loadButtonWidth =
      textWidth + buttonWidth * 0.6f;  // Increased from 0.4f

  if (ImGui::Button("Load", ImVec2(loadButtonWidth, buttonHeight))) {
    m_uiRenderer.HandleLoadImage();
  }

  ImGui::SameLine();

  textWidth = ImGui::CalcTextSize("Convert").x;
  float convertButtonWidth =
      textWidth + buttonWidth * 0.6f;  // Increased from 0.4f

  if (ImGui::Button("Convert", ImVec2(convertButtonWidth, buttonHeight))) {
    m_uiRenderer.HandleConversion();
  }

  // Put conversion type on the same line as the Convert button
  ImGui::SameLine();
}

void ControlsArea::renderConversionCombo(float comboWidth) {
  // Style the combo box
  UIStyleManager frameBg(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
  UIStyleManager button(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
  UIStyleManager popupBg(ImGuiCol_PopupBg, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
  UIStyleManager header(ImGuiCol_Header, ImVec4(0.3f, 0.3f, 0.7f, 0.8f));
  UIStyleManager headerHovered(ImGuiCol_HeaderHovered,
                               ImVec4(0.4f, 0.4f, 0.8f, 0.8f));
  UIStyleManager headerActive(ImGuiCol_HeaderActive,
                              ImVec4(0.5f, 0.5f, 0.9f, 0.8f));
  UIStyleManager text(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));

  UIStyleManager frameRounding(ImGuiStyleVar_FrameRounding, 5.0f);
  UIStyleManager popupRounding(ImGuiStyleVar_PopupRounding, 5.0f);
  UIStyleManager framePadding(ImGuiStyleVar_FramePadding, ImVec2(10, 8));
  UIStyleManager windowPadding(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));

  ImGui::PushItemWidth(comboWidth);

  if (ImGui::BeginCombo("##ConversionCombo",
                        m_state.getCurrentConversionName().c_str(),
                        ImGuiComboFlags_HeightLarge)) {
    const auto& options = m_state.conversion_registry.getOptions();
    for (int i = 0; i < options.size(); i++) {
      const bool isSelected = (m_state.current_conversion == i);
      if (ImGui::Selectable(options[i].second.c_str(), isSelected, 0,
                            ImVec2(0, 30))) {
        m_state.current_conversion = i;
      }
      if (isSelected) {
        ImGui::SetItemDefaultFocus();
      }
    }
    ImGui::EndCombo();
  }

  ImGui::PopItemWidth();
}

void ControlsArea::renderSlider(float columnWidth) {
  ImGui::PushItemWidth(columnWidth);

  // Style the slider
  UIStyleManager frameBg(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
  UIStyleManager sliderGrab(ImGuiCol_SliderGrab,
                            ImVec4(0.4f, 0.4f, 0.8f, 0.8f));
  UIStyleManager sliderGrabActive(ImGuiCol_SliderGrabActive,
                                  ImVec4(0.5f, 0.5f, 0.9f, 0.8f));
  UIStyleManager grabMinSize(ImGuiStyleVar_GrabMinSize, 10.0f);

  ImGui::SliderInt("##Slider", &m_state.slider_value, 0, 100, "%d%%");

  ImGui::PopItemWidth();
}

void ControlsArea::setupColumns(float windowWidth) {
  ImGui::Columns(2, "ControlsColumns", false);
  ImGui::SetColumnWidth(0, windowWidth * 0.7f);  // Main controls column
}

}  // namespace psm_gui
