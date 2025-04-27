#pragma once

#include <memory>
#include <string>
#include <vector>

#include "app_state.h"
#include "imgui.h"

namespace psm_gui {

// Forward declarations
class UIStyleManager;
class PreviewArea;
class ControlsArea;

/**
 * @brief Main UI renderer class that manages the application's user interface
 *
 * This class follows the Single Responsibility Principle by delegating specific
 * UI rendering tasks to specialized components.
 */
class UIRenderer {
 public:
  /**
     * @brief Construct a new UI Renderer
     *
     * @param state Reference to the application state
     */
  explicit UIRenderer(AppState& state);

  /**
     * @brief Destroy the UI Renderer
     */
  ~UIRenderer();

  // Delete copy constructor and assignment operator
  UIRenderer(const UIRenderer&) = delete;
  UIRenderer& operator=(const UIRenderer&) = delete;

  /**
     * @brief Render the main UI
     */
  void render();

  // Handle image loading
  void HandleLoadImage();

  // Handle image conversion
  void HandleConversion();

 private:
  AppState& m_state;
  std::unique_ptr<PreviewArea> m_previewArea;
  std::unique_ptr<ControlsArea> m_controlsArea;

  void setupMainWindow();
  void renderContent(float contentWidth, float contentHeight);
};

/**
 * @brief Manages ImGui style states using RAII
 */
class UIStyleManager {
 public:
  /**
     * @brief Push a style color
     *
     * @param idx Style color index
     * @param col Color value
     */
  UIStyleManager(ImGuiCol idx, const ImVec4& col);

  /**
     * @brief Push a style variable
     *
     * @param idx Style variable index
     * @param val Style variable value
     */
  UIStyleManager(ImGuiStyleVar idx, float val);

  /**
     * @brief Push a style variable with 2D vector value
     *
     * @param idx Style variable index
     * @param val Style variable value
     */
  UIStyleManager(ImGuiStyleVar idx, const ImVec2& val);

  /**
     * @brief Destroy the UI Style Manager and pop the style
     */
  ~UIStyleManager();

 private:
  ImGuiStyleVar m_varIdx;
  ImGuiCol m_colIdx;
  bool m_isColor;
  bool m_isVec2;
};

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

/**
 * @brief Renders the controls area with buttons, dropdown and slider
 */
class ControlsArea {
 public:
  /**
     * @brief Construct a new Controls Area
     *
     * @param state Reference to the application state
     */
  explicit ControlsArea(AppState& state);

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

  void renderButtons(float buttonWidth, float buttonHeight);
  void renderConversionCombo(float comboWidth);
  void renderSlider(float columnWidth);
  void setupColumns(float windowWidth);
};

}  // namespace psm_gui
