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
  // Create top toolbar (20px height with appropriate spacing)
  /*float remainingHeight = contentHeight;*/
  const float toolbarHeight = contentHeight * 0.09f;
  const float spacing = contentHeight * 0.01f;
  const float mainContentHeight = contentHeight * 0.89f;

  // Render toolbar
  renderToolbar(contentWidth, toolbarHeight);

  // Add spacing
  ImGui::Dummy(ImVec2(0, spacing));

  // Render main content area (preview + controls)
  renderMainContent(contentWidth, mainContentHeight);

  ImGui::EndChild();  // End Content child
  ImGui::End();
}

void UIRenderer::renderToolbar(float width, float height) {
  UIStyleManager childBg(ImGuiCol_ChildBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
  UIStyleManager rounding(ImGuiStyleVar_ChildRounding, 5.0f);

  // Push style for centered button text
  UIStyleManager buttonTextAlign(ImGuiStyleVar_ButtonTextAlign,
                                 ImVec2(0.5f, 0.5f));
  UIStyleManager buttonPadding(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 4.0f));

  ImGui::BeginChild("Toolbar", ImVec2(width, height), true,
                    ImGuiWindowFlags_NoScrollbar);

  // Calculate button heights and positioning
  const float itemHeight = height * 0.35f;  // Reduced height for items
  const float verticalPadding =
      (height - itemHeight) * 0.5f;  // Center items vertically
  ImGui::SetCursorPosY(verticalPadding);

  // Color space selector dropdown (left side)
  const float dropdownWidth = width * 0.2f;
  const float buttonSpacing = width * 0.01f;

  ImGui::SetNextItemWidth(dropdownWidth);
  ImGui::AlignTextToFramePadding();  // Helps align text height with buttons
  if (ImGui::BeginCombo("##ColorSpaceSelector",
                        m_state.getCurrentConversionName().c_str())) {
    const auto& options = m_state.conversion_registry.getOptions();
    for (int i = 0; i < options.size(); i++) {
      const bool isSelected = (m_state.current_conversion == i);
      if (ImGui::Selectable(options[i].second.c_str(), isSelected)) {
        m_state.current_conversion = i;
      }
      if (isSelected) ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }

  ImGui::SameLine(0, buttonSpacing * 2);

  // Calculate adaptive button widths based on text
  float loadTextWidth = ImGui::CalcTextSize("Load Image").x;
  float loadButtonWidth = loadTextWidth + width * 0.03f;  // Add padding

  // Load button with calculated width
  if (ImGui::Button("Load Image", ImVec2(loadButtonWidth, itemHeight))) {
    HandleLoadImage();
  }

  ImGui::SameLine(0, buttonSpacing);

  // Convert button (changed from Export)
  float convertTextWidth = ImGui::CalcTextSize("Convert").x;
  float convertButtonWidth = convertTextWidth + width * 0.03f;  // Add padding

  if (ImGui::Button("Convert", ImVec2(convertButtonWidth, itemHeight))) {
    // Handle conversion functionality
    HandleConversion();
  }

  ImGui::EndChild();
}

void UIRenderer::renderMainContent(float width, float height) {
  // Define the left panel width
  const float leftPanelWidth = width * 0.25f;
  const float infoBoxHeight = height * 0.20f;
  const float controlsHeight =
      height - infoBoxHeight - ImGui::GetStyle().ItemSpacing.y;

  // Left panel with controls
  ImGui::BeginChild("LeftPanel", ImVec2(leftPanelWidth, controlsHeight), false,
                    ImGuiWindowFlags_NoScrollbar);

  // Dynamic controls section
  ImGui::BeginChild("ControlsPanel", ImVec2(0, 0), true,
                    ImGuiWindowFlags_NoScrollbar);
  renderDynamicControls(leftPanelWidth, controlsHeight);
  ImGui::EndChild();

  ImGui::EndChild();

  // Preview area (right side of controls)
  const float previewWidth =
      width - leftPanelWidth - ImGui::GetStyle().ItemSpacing.x;
  ImGui::SameLine();

  ImGui::BeginChild("PreviewArea", ImVec2(previewWidth, controlsHeight), true,
                    ImGuiWindowFlags_NoScrollbar);
  renderBeforeAfterPreview(previewWidth, controlsHeight);
  ImGui::EndChild();

  // Move to bottom row - now the info panel takes the full bottom left area below controls
  ImGui::BeginChild("InfoPanel", ImVec2(leftPanelWidth, infoBoxHeight), true,
                    ImGuiWindowFlags_NoScrollbar);
  renderInfoPanel(leftPanelWidth, infoBoxHeight);
  ImGui::EndChild();

  // Film strip now aligns with preview area
  ImGui::SameLine();
  ImGui::BeginChild("FilmStrip", ImVec2(previewWidth, infoBoxHeight), true,
                    ImGuiWindowFlags_NoScrollbar);
  float filmWidth = previewWidth * 0.98f;
  float filmHeight = infoBoxHeight * 0.90f;
  renderFilmStrip(filmWidth, filmHeight);
  ImGui::EndChild();
}

void UIRenderer::renderFilmStrip(float width, float height) {
  UIStyleManager childBg(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
  UIStyleManager rounding(ImGuiStyleVar_ChildRounding, 5.0f);

  ImGui::BeginChild("FilmStrip", ImVec2(width, height), true,
                    ImGuiWindowFlags_NoScrollbar);

  // Add thumbnails for batch processing
  const float thumbSize = height * 0.7f;
  const float spacing = width * 0.01f;
  float posX = spacing;

  ImGui::SetCursorPos(ImVec2(posX, (height - thumbSize) * 0.5f));

  // Placeholder for the film strip thumbnails
  for (int i = 0; i < 5; i++) {
    ImGui::SetCursorPosX(posX);
    ImGui::Button(("Sample " + std::to_string(i + 1)).c_str(),
                  ImVec2(thumbSize * 1.5f, thumbSize));
    posX += thumbSize * 1.5f + spacing;
    ImGui::SameLine();
  }

  ImGui::EndChild();
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

    // Extract filename from path for display
    size_t lastSlash = filePath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
      m_state.fileName = filePath.substr(lastSlash + 1);
    } else {
      m_state.fileName = filePath;
    }

    // Load the new image
    if (LoadTextureFromFile(filePath.c_str(), &m_state.image_texture,
                            &m_state.image_width, &m_state.image_height)) {
      m_state.has_image = true;

      // Calculate file size in MB
      // This is approximate and should be replaced with actual file size calculation
      m_state.fileSizeMB =
          (float)(m_state.image_width * m_state.image_height * 3) /
          (1024.0f * 1024.0f);
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

void UIRenderer::renderDynamicControls(float width, float height) {
  UIStyleManager frameBg(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
  UIStyleManager sliderGrab(ImGuiCol_SliderGrab,
                            ImVec4(0.4f, 0.4f, 0.8f, 0.8f));
  UIStyleManager sliderGrabActive(ImGuiCol_SliderGrabActive,
                                  ImVec4(0.5f, 0.5f, 0.9f, 0.8f));

  // Use vertical centering for the entire control panel
  const float totalContentHeight =
      height * 0.6f;  // Estimate total content height
  const float startY = (height - totalContentHeight) * 0.5f;
  ImGui::SetCursorPosY(startY);

  const float sliderWidth = width * 0.85f;
  const float sliderHeight = height * 0.1f;  // Reduced slider height

  // Center sliders horizontally
  const float startX = (width - sliderWidth) * 0.5f;
  ImGui::SetCursorPosX(startX);

  ImGui::PushItemWidth(sliderWidth);

  // Dynamic slider controls based on selected color space
  switch (m_state.getCurrentConversionType()) {
    case ConversionType::ORGB:
      ImGui::Text("Chromaticity");
      ImGui::SetCursorPosX(startX);
      ImGui::SliderInt("##Chromaticity", &m_state.oRGB_sliders.chromaticity, 0,
                       100, "%d%%");
      ImGui::SetCursorPosX(startX);
      ImGui::Text("-100");
      ImGui::SameLine(startX + sliderWidth - ImGui::CalcTextSize("100").x);
      ImGui::Text("100");

      ImGui::Dummy(ImVec2(0, height * 0.05f));

      ImGui::SetCursorPosX(startX);
      ImGui::Text("Cool-Warm");
      ImGui::SetCursorPosX(startX);
      ImGui::SliderInt("##CoolWarm", &m_state.oRGB_sliders.coolWarm, 0, 100,
                       "%d%%");
      ImGui::SetCursorPosX(startX);
      ImGui::Text("Cool");
      ImGui::SameLine(startX + sliderWidth - ImGui::CalcTextSize("Warm").x);
      ImGui::Text("Warm");
      break;

    case ConversionType::ADOBE_RGB:
      ImGui::SetCursorPosX(startX);
      ImGui::Text("Saturation");
      ImGui::SetCursorPosX(startX);
      ImGui::SliderInt("##Saturation", &m_state.adobeRGB_sliders.saturation, 0,
                       100, "%d%%");
      ImGui::SetCursorPosX(startX);
      ImGui::Text("0%%");
      ImGui::SameLine(startX + sliderWidth - ImGui::CalcTextSize("100%").x);
      ImGui::Text("100%%");

      ImGui::Dummy(ImVec2(0, height * 0.05f));

      ImGui::SetCursorPosX(startX);
      ImGui::Text("Tone");
      ImGui::SetCursorPosX(startX);
      ImGui::SliderInt("##Tone", &m_state.adobeRGB_sliders.tone, 0, 100,
                       "%d%%");
      ImGui::SetCursorPosX(startX);
      ImGui::Text("Dark");
      ImGui::SameLine(startX + sliderWidth - ImGui::CalcTextSize("Light").x);
      ImGui::Text("Light");
      break;

    // Add cases for other color spaces
    default:
      ImGui::SetCursorPosX(startX);
      ImGui::Text("Parameter 1");
      ImGui::SetCursorPosX(startX);
      ImGui::SliderInt("##Param1", &m_state.slider_value, 0, 100, "%d%%");

      ImGui::Dummy(ImVec2(0, height * 0.05f));

      ImGui::SetCursorPosX(startX);
      ImGui::Text("Parameter 2");
      ImGui::SetCursorPosX(startX);
      ImGui::SliderInt("##Param2", &m_state.vertical_slider_value, 0, 100,
                       "%d%%");
      break;
  }

  ImGui::PopItemWidth();
}

void UIRenderer::renderInfoPanel(float width, float height) {
  // Style for the info box
  UIStyleManager childBg(ImGuiCol_ChildBg, ImVec4(0.12f, 0.12f, 0.15f, 1.0f));

  // Display file info
  if (m_state.has_image) {
    ImGui::Text("File: %s", m_state.fileName.empty()
                                ? "Untitled"
                                : m_state.fileName.c_str());
    ImGui::Text("Size: %.2f MB", m_state.fileSizeMB);
    ImGui::Text("Resolution: %dx%d", m_state.image_width, m_state.image_height);
    ImGui::Text("Color Space: %s", m_state.getCurrentConversionName().c_str());

    ImGui::Dummy(ImVec2(0, height * 0.05f));

    // Push centered button text style and padding to match toolbar
    UIStyleManager buttonTextAlign(ImGuiStyleVar_ButtonTextAlign,
                                   ImVec2(0.5f, 0.5f));
    UIStyleManager buttonPadding(ImGuiStyleVar_FramePadding,
                                 ImVec2(8.0f, 4.0f));

    // Calculate adaptive button widths based on text - like in the toolbar
    float resetTextWidth = ImGui::CalcTextSize("↺ Reset").x;
    float resetButtonWidth = resetTextWidth + width * 0.05f;  // Add padding

    if (ImGui::Button("↺ Reset", ImVec2(resetButtonWidth, 0))) {
      // Reset sliders based on color space
      resetSliders();
    }

    ImGui::SameLine();

    float snapshotTextWidth = ImGui::CalcTextSize("📸 Snapshot").x;
    float snapshotButtonWidth =
        snapshotTextWidth + width * 0.05f;  // Add padding

    if (ImGui::Button("📸 Snapshot", ImVec2(snapshotButtonWidth, 0))) {
      // Capture the current "after" view
      captureSnapshot();
    }
  } else {
    ImGui::Text("No image loaded");
  }
}

void UIRenderer::renderBeforeAfterPreview(float width, float height) {
  if (!m_state.has_image || m_state.image_texture == 0) {
    // Display placeholder
    const float posX = width / 2 - 50;
    const float posY = height / 2 - 10;
    ImGui::SetCursorPos(ImVec2(posX, posY));
    ImGui::Text("No image loaded");
    return;
  }

  // Calculate the divider position
  float dividerPos = (width * m_state.beforeAfterDivider) / 100.0f;

  // Render the "before" image (original)
  float displayWidth, displayHeight;
  calculateImageDimensions(width, height, m_state.image_width,
                           m_state.image_height, displayWidth, displayHeight);

  // Center the image vertically
  float posY = (height - displayHeight) * 0.5f;
  ImGui::SetCursorPos(ImVec2(0, posY));

  // The "before" image is clipped at the divider position
  ImVec2 imageSize(displayWidth, displayHeight);
  ImVec2 uv0(0, 0);
  ImVec2 uv1((float)m_state.beforeAfterDivider / 100.0f, 1);
  ImGui::Image((void*)(intptr_t)m_state.image_texture,
               ImVec2(dividerPos, displayHeight), uv0, uv1);

  // TODO: Render the "after" image when processing is implemented
  // For now, use the same image for demonstration
  ImGui::SetCursorPos(ImVec2(dividerPos, posY));
  uv0 = ImVec2((float)m_state.beforeAfterDivider / 100.0f, 0);
  uv1 = ImVec2(1, 1);
  ImGui::Image((void*)(intptr_t)m_state.image_texture,
               ImVec2(width - dividerPos, displayHeight), uv0, uv1);

  // Draw the divider line
  ImVec2 start(dividerPos, posY);
  ImVec2 end(dividerPos, posY + displayHeight);
  ImVec2 windowPos = ImGui::GetWindowPos();

  ImGui::GetWindowDrawList()->AddLine(
      ImVec2(windowPos.x + start.x, windowPos.y + start.y),
      ImVec2(windowPos.x + end.x, windowPos.y + end.y),
      IM_COL32(255, 255, 255, 200), 2.0f);

  // Make the divider draggable
  ImGui::SetCursorPos(ImVec2(dividerPos - 5, posY));
  ImGui::InvisibleButton("divider", ImVec2(10, displayHeight));
  if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) {
    float deltaX = ImGui::GetIO().MouseDelta.x;
    dividerPos += deltaX;
    dividerPos = std::min(std::max(dividerPos, 10.0f), width - 10.0f);
    m_state.beforeAfterDivider = (int)((dividerPos / width) * 100.0f);
  }

  // Change cursor when hovering over the divider
  if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
}

void UIRenderer::resetSliders() {
  // Reset sliders based on current color space
  switch (m_state.getCurrentConversionType()) {
    case ConversionType::ORGB:
      m_state.oRGB_sliders.chromaticity = 50;
      m_state.oRGB_sliders.coolWarm = 50;
      break;
    case ConversionType::ADOBE_RGB:
      m_state.adobeRGB_sliders.saturation = 50;
      m_state.adobeRGB_sliders.tone = 50;
      break;
    // Add cases for other color spaces
    default:
      m_state.slider_value = 50;
      m_state.vertical_slider_value = 50;
      break;
  }
}

void UIRenderer::captureSnapshot() {
  // TODO: Implement snapshot functionality
  // This would save the current processed image
}

void UIRenderer::calculateImageDimensions(float containerWidth,
                                          float containerHeight, int imageWidth,
                                          int imageHeight, float& displayWidth,
                                          float& displayHeight) {
  // Calculate aspect ratio to maintain proportions
  float imageAspect =
      static_cast<float>(imageWidth) / static_cast<float>(imageHeight);
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

}  // namespace psm_gui
