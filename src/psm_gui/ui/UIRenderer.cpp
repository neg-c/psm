#include "UIRenderer.hpp"

#include <imgui.h>

namespace psm_gui::ui {
UIRenderer::UIRenderer(AppState& state) : state_(state) {}

void UIRenderer::render() {
  ImGui::NewFrame();
  /*ImGui::ShowAboutWindow();*/
  /*const ImGuiViewport* viewport = ImGui::GetMainViewport();*/
  /*ImGui::SetNextWindowPos(viewport->Pos);*/
  /*ImGui::SetNextWindowSize(viewport->Size);*/
  /*ImGui::ShowAboutWindow();*/

  auto* vp = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(vp->WorkPos);
  ImGui::SetNextWindowSize(vp->WorkSize);
  ImGui::Begin("##DockSpaceHost", nullptr,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking);
  /*ImGuiWindowFlags_NoDecoration);*/
  auto dockId = ImGui::GetID("MyDockSpace");
  ImGui::DockSpace(dockId, {0, 0});
  ImGui::End();

  /*// 2) now each panel is just its own window*/
  ImGui::Begin("Toolbar");
  ImGui::End();

  ImGui::Begin("Control Panel");
  ImGui::End();

  ImGui::Begin("Preview");

  ImGui::DockBuilderRemoveNode(dockId);
  ImGui::DockBuilderAddNode(dockId, ImGuiDockNodeFlags_DockSpace);
  ImGui::DockBuilderSetNodeSize(dockId, vp->WorkSize);

  // split the node: top for toolbar, left for control, remainder for preview
  auto dock_main_id = dockId;
  auto dock_id_top = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up,
                                                 0.05f, nullptr, &dock_main_id);
  auto dock_id_left = ImGui::DockBuilderSplitNode(
      dock_main_id, ImGuiDir_Left, 0.25f, nullptr, &dock_main_id);

  // assign windows
  ImGui::DockBuilderDockWindow("Toolbar", dock_id_top);
  ImGui::DockBuilderDockWindow("Control Panel", dock_id_left);
  ImGui::DockBuilderDockWindow("Preview", dock_main_id);
  ImGui::DockBuilderFinish(dockId);
  ImGui::Render();
}

}  // namespace psm_gui::ui
