#include "Toolbar.hpp"

#include <imgui.h>

namespace psm_gui::ui::panels {

void Toolbar::draw(AppState& s, const PanelRect& r) {
  ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
                           ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
  ImGui::SetNextWindowPos(r.pos);
  ImGui::SetNextWindowSize(r.size);
  ImGui::Begin("Toolbar", nullptr, flags);

  ImGuiStyle& style = ImGui::GetStyle();
  float btnW = 80.0f;
  float spacing = style.ItemSpacing.x;
  float totalW = (btnW * 2) + spacing;
  float comboW = r.size.x * 0.3f;

  float contentHeight = r.size.y - (style.WindowPadding.y * 2);
  float frameHeight = ImGui::GetFrameHeight();
  float offsetY =
      style.WindowPadding.y + ((contentHeight - frameHeight) * 0.5f);
  ImGui::SetCursorPosY(offsetY);

  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 7.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
  if (ImGui::BeginTable("##ToolbarTable", 3, ImGuiTableFlags_NoPadInnerX)) {
    ImGui::TableSetupColumn("##Combo", ImGuiTableColumnFlags_WidthFixed,
                            comboW);
    ImGui::TableSetupColumn("##Filler", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("##Buttons", ImGuiTableColumnFlags_WidthFixed,
                            totalW);
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.0f);
    ImGui::PushItemWidth(-FLT_MIN);
    static const char* names[] = {"sRGB", "AdobeRGB", "DisplayP3"};
    static int current_item = 0;
    ImGui::Combo("Color Space", &current_item, names, IM_ARRAYSIZE(names));
    ImGui::PopItemWidth();

    ImGui::TableNextColumn();

    ImGui::TableNextColumn();
    float rightPadding = -20.0f;
    float buttonsWidth = totalW - rightPadding;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (totalW - buttonsWidth));
    if (ImGui::Button("Reset", ImVec2(btnW, 0))) { /*…*/
    }
    ImGui::SameLine();
    if (ImGui::Button("Save", ImVec2(btnW, 0))) { /*…*/
    }
    ImGui::PopStyleVar(2);

    ImGui::EndTable();
  }

  ImGui::End();
}

}  // namespace psm_gui::ui::panels
