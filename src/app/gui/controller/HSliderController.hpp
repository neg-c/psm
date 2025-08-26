#pragma once

#include "app/AppState.hpp"

namespace psm_gui::controller {

class HSliderController {
 public:
  explicit HSliderController(AppState &state);
  void updateImage();  // Updates both R and B channels together

 private:
  AppState &state_;
};
}  // namespace psm_gui::controller
