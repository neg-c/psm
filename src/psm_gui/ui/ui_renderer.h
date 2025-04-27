#pragma once

#include <memory>
#include <string>
#include <vector>

#include "app/app_state.h"
#include "imgui.h"
#include "style/style_manager.h"
#include "ui/controls_area.h"
#include "ui/preview_area.h"

namespace psm_gui {

// Forward declarations
class UIStyleManager;

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
  void renderToolbar(float width, float height);
  void renderMainContent(float width, float height);
  void renderFilmStrip(float width, float height);
  void renderDynamicControls(float width, float height);
  void renderInfoPanel(float width, float height);
  void renderBeforeAfterPreview(float width, float height);

  void resetSliders();
  void captureSnapshot();
  void calculateImageDimensions(float containerWidth, float containerHeight,
                                int imageWidth, int imageHeight,
                                float& displayWidth, float& displayHeight);
};

}  // namespace psm_gui
