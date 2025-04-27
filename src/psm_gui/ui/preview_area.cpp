#include "preview_area.h"

#include <imgui.h>

#include "style/style_manager.h"

namespace psm_gui {

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

}  // namespace psm_gui
