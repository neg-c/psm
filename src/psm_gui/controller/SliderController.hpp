#pragma once

#include "app/AppState.hpp"

namespace psm_gui::controller {

class SliderController {
 public:
  explicit SliderController(AppState &state);
  void updateImage();

 private:
  AppState &state_;
};
}  // namespace psm_gui::controller
