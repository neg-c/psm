#pragma once

#include <GL/glew.h>
#include <imgui.h>

#include "PanelRect.hpp"
#include "app/AppState.hpp"

namespace psm_gui::ui::panels {
struct PreviewArea {
  static void draw(AppState& s, const PanelRect& r);

 private:
  static void DrawMagnifyingGlass(AppState& s, GLuint texture_id,
                                  const ImVec2& mouse_pos,
                                  const ImVec2& image_pos,
                                  const ImVec2& image_size, int pixel_x,
                                  int pixel_y);
};
}  // namespace psm_gui::ui::panels
