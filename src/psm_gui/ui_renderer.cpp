#include "ui_renderer.h"

// Forward declaration of the handler functions
extern void HandleLoadImage(psm_gui::AppState& state);
extern void HandleConversion(psm_gui::AppState& state);

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

  // Disable scrolling in the preview area
  ImGui::BeginChild(
      "Preview", ImVec2(width, height), true,
      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

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

  // Add a border around the image for better visibility
  ImVec2 imagePos = ImGui::GetCursorScreenPos();
  ImVec2 imageEnd(imagePos.x + displayWidth, imagePos.y + displayHeight);

  // Draw the image
  ImGui::Image((void*)(intptr_t)m_state.image_texture,
               ImVec2(displayWidth, displayHeight));

  // Draw a subtle border around the image
  ImGui::GetWindowDrawList()->AddRect(
      imagePos, imageEnd,
      IM_COL32(180, 180, 180, 100),  // Light gray, semi-transparent
      0.0f,                          // rounding
      0,                             // flags
      1.0f);                         // thickness
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

  // Always fit the image within the container while maintaining aspect ratio
  if (imageAspect > previewAspect) {
    // Image is wider than preview area - fit to width
    displayWidth = containerWidth;
    displayHeight = displayWidth / imageAspect;
  } else {
    // Image is taller than preview area - fit to height
    displayHeight = containerHeight;
    displayWidth = displayHeight * imageAspect;
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
    HandleLoadImage(m_state);
  }

  ImGui::SameLine();

  textWidth = ImGui::CalcTextSize("Convert").x;
  float convertButtonWidth =
      textWidth + buttonWidth * 0.6f;  // Increased from 0.4f

  if (ImGui::Button("Convert", ImVec2(convertButtonWidth, buttonHeight))) {
    HandleConversion(m_state);
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

void UIRenderer::HandleLoadImage() {
  // Open file dialog to select an image
  std::string filePath = OpenImageFileDialog();

  if (!filePath.empty()) {
    // Clean up previous texture if it exists
    if (m_state.image_texture) {
      glDeleteTextures(1, &m_state.image_texture);
      m_state.image_texture = 0;
    }

    // Load the new image
    if (LoadTextureFromFile(filePath.c_str(), &m_state.image_texture,
                            &m_state.image_width, &m_state.image_height)) {
      m_state.has_image = true;
    } else {
      std::cerr << "Failed to load image: " << filePath << std::endl;
    }
  }
}

void UIRenderer::HandleConversion() {
  if (!m_state.has_image || m_state.image_texture == 0) {
    return;
  }

  // Get the image data from the texture
  int width = m_state.image_width;
  int height = m_state.image_height;
  std::vector<unsigned char> imageData(width * height * 3);

  // Bind the texture to read its data
  glBindTexture(GL_TEXTURE_2D, m_state.image_texture);
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData.data());

  // Process the image
  if (ImageProcessor::ProcessImage(m_state, imageData, width, height, 3)) {
    // Update the texture with the processed image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, imageData.data());
  }
}

}  // namespace psm_gui
