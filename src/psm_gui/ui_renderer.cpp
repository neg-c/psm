#include "ui_renderer.h"

namespace psm_gui {

// UIStyleManager implementation
UIStyleManager::UIStyleManager(ImGuiCol idx, const ImVec4& col)
    : m_varIdx(ImGuiStyleVar_COUNT),
      m_colIdx(idx),
      m_isColor(true),
      m_isVec2(false) {
  ImGui::PushStyleColor(idx, col);
}

UIStyleManager::UIStyleManager(ImGuiStyleVar idx, float val)
    : m_varIdx(idx),
      m_colIdx(ImGuiCol_COUNT),
      m_isColor(false),
      m_isVec2(false) {
  ImGui::PushStyleVar(idx, val);
}

UIStyleManager::UIStyleManager(ImGuiStyleVar idx, const ImVec2& val)
    : m_varIdx(idx),
      m_colIdx(ImGuiCol_COUNT),
      m_isColor(false),
      m_isVec2(true) {
  ImGui::PushStyleVar(idx, val);
}

UIStyleManager::~UIStyleManager() {
  if (m_isColor) {
    ImGui::PopStyleColor();
  } else {
    ImGui::PopStyleVar();
  }
}

// PreviewArea implementation
PreviewArea::PreviewArea(AppState& state) : m_state(state) {}

void PreviewArea::render(float width, float height) {
  // Style the preview area
  UIStyleManager bgColor(ImGuiCol_ChildBg,
                         ImVec4(0.184f, 0.184f, 0.184f, 1.0f));
  UIStyleManager rounding(ImGuiStyleVar_ChildRounding, 15.0f);

  ImGui::BeginChild("Preview", ImVec2(width, height), true);

  if (m_state.has_image && m_state.image_texture) {
    renderImage(width, height);
  } else {
    renderPlaceholder(width, height);
  }

  ImGui::EndChild();
}

void PreviewArea::renderImage(float width, float height) {
  float displayWidth, displayHeight;
  calculateImageDimensions(width, height, displayWidth, displayHeight);

  // Center the image
  float posX = (width - displayWidth) * 0.5f;
  float posY = (height - displayHeight) * 0.5f;

  ImGui::SetCursorPos(ImVec2(posX, posY));
  ImGui::Image((void*)(intptr_t)m_state.image_texture,
               ImVec2(displayWidth, displayHeight));
}

void PreviewArea::renderPlaceholder(float width, float height) {
  const float posX = width / 2 - 50;
  const float posY = height / 2 - 10;
  ImGui::SetCursorPos(ImVec2(posX, posY));
  ImGui::Text("No image loaded");
}

void PreviewArea::calculateImageDimensions(float containerWidth,
                                           float containerHeight,
                                           float& displayWidth,
                                           float& displayHeight) {
  // Calculate aspect ratio to maintain proportions
  float imageAspect = static_cast<float>(m_state.image_width) /
                      static_cast<float>(m_state.image_height);
  float previewAspect = containerWidth / containerHeight;

  if (imageAspect > previewAspect) {
    // Image is wider than preview area
    displayWidth = containerWidth;
    displayHeight = containerWidth / imageAspect;
  } else {
    // Image is taller than preview area
    displayHeight = containerHeight;
    displayWidth = containerHeight * imageAspect;
  }
}

// ControlsArea implementation
ControlsArea::ControlsArea(AppState& state) : m_state(state) {}

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
    // Load button clicked
  }

  ImGui::SameLine();

  textWidth = ImGui::CalcTextSize("Convert").x;
  float convertButtonWidth =
      textWidth + buttonWidth * 0.6f;  // Increased from 0.4f

  if (ImGui::Button("Convert", ImVec2(convertButtonWidth, buttonHeight))) {
    // Convert button clicked
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

  if (ImGui::BeginCombo(
          "##ConversionCombo",
          m_state.conversion_options[m_state.current_conversion].c_str(),
          ImGuiComboFlags_HeightLarge)) {
    for (int i = 0; i < m_state.conversion_options.size(); i++) {
      const bool isSelected = (m_state.current_conversion == i);
      if (ImGui::Selectable(m_state.conversion_options[i].c_str(), isSelected,
                            0, ImVec2(0, 30))) {
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

// UIRenderer implementation
UIRenderer::UIRenderer(AppState& state)
    : m_state(state),
      m_previewArea(std::make_unique<PreviewArea>(state)),
      m_controlsArea(std::make_unique<ControlsArea>(state)) {}

UIRenderer::~UIRenderer() = default;

void UIRenderer::render() {
  setupMainWindow();

  // Get the current window size for scaling calculations
  const float windowWidth = ImGui::GetWindowWidth();
  const float windowHeight = ImGui::GetWindowHeight();

  // Calculate padding based on window size (responsive)
  const float paddingX = windowWidth * 0.025f;
  const float paddingY = windowHeight * 0.03f;

  // Add padding for content
  UIStyleManager framePadding(ImGuiStyleVar_FramePadding,
                              ImVec2(paddingX, paddingY));

  // Set content region with padding
  ImGui::SetCursorPos(ImVec2(paddingX, paddingY));
  const float contentWidth = windowWidth - (paddingX * 2);
  const float contentHeight = windowHeight - (paddingY * 2);

  renderContent(contentWidth, contentHeight);
}

void UIRenderer::setupMainWindow() {
  // Set window to be fullscreen with padding
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

  const ImGuiWindowFlags windowFlags =
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoScrollbar |
      ImGuiWindowFlags_NoScrollWithMouse;

  // Set padding to zero to avoid extra space
  UIStyleManager windowPadding(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

  ImGui::Begin("PSM GUI", nullptr, windowFlags);
}

void UIRenderer::renderContent(float contentWidth, float contentHeight) {
  ImGui::BeginChild(
      "Content", ImVec2(contentWidth, contentHeight), false,
      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

  const float windowHeight = ImGui::GetWindowHeight();

  const float controlsHeight = windowHeight * 0.10f;  // Reduced from 0.12f
  const float spacing = windowHeight * 0.02f;

  const float previewHeight =
      ImGui::GetContentRegionAvail().y - controlsHeight - spacing;
  const float previewWidth = ImGui::GetContentRegionAvail().x;

  // Calculate the width for the vertical slider - increase to match horizontal slider height
  const float verticalSliderWidth =
      contentWidth * 0.07f;  // Increased from 0.05f
  const float horizontalSpacing = contentWidth * 0.02f;
  const float adjustedPreviewWidth =
      previewWidth - verticalSliderWidth - horizontalSpacing;

  m_controlsArea->renderVerticalSlider(verticalSliderWidth, previewHeight);

  ImGui::SameLine();
  ImGui::Dummy(ImVec2(horizontalSpacing, 0));
  ImGui::SameLine();

  m_previewArea->render(adjustedPreviewWidth, previewHeight);

  // Vertical spacer
  ImGui::Dummy(ImVec2(0, spacing));

  m_controlsArea->render(contentWidth, controlsHeight);

  ImGui::EndChild();  // End Content child

  ImGui::End();
}

}  // namespace psm_gui
