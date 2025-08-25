#pragma once

#include "app/AppState.hpp"

namespace psm_gui::controller {

class ToolbarController {
 public:
  explicit ToolbarController(AppState &state);
  void loadImage();
  void saveImage();
  void updateColorSpace(int colorspace);
  void convertImage();

 private:
  AppState &state_;
};

}  // namespace psm_gui::controller
