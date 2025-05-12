#include "Toolbar.hpp"

namespace psm_gui::ui::panels {

void Toolbar::draw(AppState& s, const PanelRect& r) {
  ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
                           ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
  ImGui::SetNextWindowPos(r.pos);
  ImGui::SetNextWindowSize(r.size);
  ImGui::Begin("Toolbar", nullptr, flags);
  // … buttons, menus, etc …
  ImGui::End();
}

}  // namespace psm_gui::ui::panels
