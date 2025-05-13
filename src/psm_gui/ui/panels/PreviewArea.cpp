#include "panels/PreviewArea.hpp"

#include "PreviewArea.hpp"

namespace psm_gui::ui::panels {

void PreviewArea::draw(AppState& s, const PanelRect& r) {
  ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
                           ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
  ImGui::SetNextWindowPos(r.pos);
  ImGui::SetNextWindowSize(r.size);
  ImGui::Begin("PreviewArea", nullptr, flags);
  // … buttons, menus, etc …
  ImGui::End();
}

}  // namespace psm_gui::ui::panels
