#pragma once

#include <imgui.h>

#include "app/app_state.h"

// Forward declaration
namespace psm_gui {
class UIRenderer;

class ControlsArea {
 public:
  /**
     * @brief Construct a new Controls Area
     *
     * @param state Reference to the application state
     * @param uiRenderer Reference to the UI renderer
     */
  ControlsArea(AppState& state, UIRenderer& uiRenderer);

  /**
   * @brief Render the controls area
   *
   * @param windowWidth Width of the window
   * @param windowHeight Height of the window
   */
  void render(float windowWidth, float windowHeight);

  /**
   * @brief Render the vertical slider
   *
   * @param width Width of the slider area
   * @param height Height of the slider area
   */
  void renderVerticalSlider(float width, float height);

 private:
  AppState& m_state;
  UIRenderer& m_uiRenderer;

  void renderButtons(float buttonWidth, float buttonHeight);
  void renderConversionCombo(float comboWidth);
  void renderSlider(float columnWidth);
  void setupColumns(float windowWidth);
};

}  // namespace psm_gui
