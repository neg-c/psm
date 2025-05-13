#include "HorizontalSlider.hpp"

namespace psm_gui::ui::panels {

void HorizontalSlider::draw(AppState& s, const PanelRect& r) {
  ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
                           ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
  ImGui::SetNextWindowPos(r.pos);
  ImGui::SetNextWindowSize(r.size);
  ImGui::Begin("HorizontalSlider", nullptr, flags);
  // … buttons, menus, etc …
  ImGui::End();
}

}  // namespace psm_gui::ui::panels
