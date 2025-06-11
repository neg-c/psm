#pragma once

#include "PanelRect.hpp"
#include "app/AppState.hpp"
#include <imgui.h>

namespace psm_gui::ui::panels {
struct PreviewArea {
  static void draw(AppState& s, const PanelRect& r);
  
private:
  static void DrawMagnifyingGlass(
      AppState& s, const ImVec2& mouse_pos, const ImVec2& image_pos, 
      const ImVec2& image_size, int pixel_x, int pixel_y);
};
}  // namespace psm_gui::ui::panels
