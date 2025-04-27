#pragma once

#include <imgui.h>

#include "app/app_state.h"

namespace psm_gui {

/**
 * @brief Renders the preview area with image or placeholder
 */
class PreviewArea {
 public:
  /**
   * @brief Construct a new Preview Area
   *
   * @param state Reference to the application state
   */
  explicit PreviewArea(AppState& state);

  /**
   * @brief Render the preview area
   *
   * @param width Width of the preview area
   * @param height Height of the preview area
   */
  void render(float width, float height);

 private:
  AppState& m_state;

  void renderImage(float width, float height);
  void renderPlaceholder(float width, float height);
  void calculateImageDimensions(float containerWidth, float containerHeight,
                                float& displayWidth, float& displayHeight);
};

}  // namespace psm_gui
