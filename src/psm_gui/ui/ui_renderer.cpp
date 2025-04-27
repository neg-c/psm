#include "ui_renderer.h"

#include <iostream>

#include "style/style_manager.h"
#include "utils/file_dialog.h"
#include "utils/image_loader.h"
#include "utils/image_processor.h"

namespace psm_gui {

// UIRenderer implementation
UIRenderer::UIRenderer(AppState& state) : m_state(state) {
  m_previewArea = std::make_unique<PreviewArea>(state);
  m_controlsArea = std::make_unique<ControlsArea>(state, *this);
}

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
