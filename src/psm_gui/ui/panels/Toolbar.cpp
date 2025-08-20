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
  float btnW = 100.0f;
  float spacing = style.ItemSpacing.x;
  float totalW = (btnW * 2) + spacing;
  float comboW = r.size.x * 0.35f;

  float contentHeight = r.size.y - (style.WindowPadding.y * 2);
  float frameHeight = ImGui::GetFrameHeight();
  float offsetY =
      style.WindowPadding.y + ((contentHeight - frameHeight) * 0.3f);
  ImGui::SetCursorPosY(offsetY);

  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 10));
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.7f, 0.9f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.8f, 1.0f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.6f, 0.8f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 0.8f));
  ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,
                        ImVec4(0.15f, 0.15f, 0.15f, 0.9f));
  ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.15f, 0.15f, 0.15f, 0.94f));

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

    if (ImGui::Combo("##ColorSpace", &s.selected_colorspace, names,
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

    ImGui::EndTable();
  }

  ImGui::PopStyleVar(2);
  ImGui::PopStyleColor(8);

  ImGui::End();
}

}  // namespace psm_gui::ui::panels
