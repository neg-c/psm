#pragma once

#include "app/AppState.hpp"

namespace psm_gui::controller {

class VSliderController {
 public:
  explicit VSliderController(AppState &state);
  void updateImage();  // Updates both R and B channels together

 private:
  AppState &state_;
};
}  // namespace psm_gui::controller
