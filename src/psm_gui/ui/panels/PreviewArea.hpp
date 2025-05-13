#pragma once

#include "PanelRect.hpp"
#include "app/AppState.hpp"

namespace psm_gui::ui::panels {
struct PreviewArea {
  static void draw(AppState& s, const PanelRect& r);
};
}  // namespace psm_gui::ui::panels
