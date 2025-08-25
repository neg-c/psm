#pragma once

#include "PanelRect.hpp"
#include "app/AppState.hpp"
#include "controller/ToolbarController.hpp"

namespace psm_gui::ui::panels {
struct Toolbar {
  static void draw(AppState& s, const PanelRect& r);
};
}  // namespace psm_gui::ui::panels
