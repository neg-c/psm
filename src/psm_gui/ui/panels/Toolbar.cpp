#include "Toolbar.hpp"

#include <imgui.h>

#include <iostream>

namespace psm_gui::ui::panels {

void Toolbar::draw(AppState& s, const PanelRect& r) {
  controller::ToolbarController toolbarCtl_(s);

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
      style.WindowPadding.y + ((contentHeight - frameHeight) * 0.3f);
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
    static const char* names[] = {"sRGB", "AdobeRGB", "DisplayP3", "oRGB"};

    if (ImGui::Combo("Color Space", &s.selected_colorspace, names,
                     IM_ARRAYSIZE(names))) {
      toolbarCtl_.updateColorSpace(s.selected_colorspace);
    }
    ImGui::PopItemWidth();

    ImGui::TableNextColumn();

    ImGui::TableNextColumn();
    float rightPadding = -20.0f;
    float buttonsWidth = totalW - rightPadding;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (totalW - buttonsWidth));
    if (ImGui::Button("Load", ImVec2(btnW, 0))) {
      toolbarCtl_.loadImage();
    }
    ImGui::SameLine();
    if (ImGui::Button("Save", ImVec2(btnW, 0))) {
      toolbarCtl_.saveImage();
    }
    ImGui::PopStyleVar(2);

    ImGui::EndTable();
  }

  ImGui::End();
}

}  // namespace psm_gui::ui::panels
