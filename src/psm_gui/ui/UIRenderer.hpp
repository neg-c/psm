#pragma once

#include "app/AppState.hpp"

namespace psm_gui::ui {

class UIRenderer {
 public:
  explicit UIRenderer(AppState& state);
  void render();

 private:
  AppState state_;
};

}  // namespace psm_gui::ui
