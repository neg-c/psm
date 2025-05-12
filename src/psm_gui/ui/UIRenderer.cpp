#include "UIRenderer.hpp"

#include <imgui_internal.h>

#include "panels/PanelRect.hpp"
#include "panels/Toolbar.hpp"

namespace psm_gui::ui {
UIRenderer::UIRenderer(AppState& state) : state_(state) {}

void UIRenderer::render() {
  ImGui::NewFrame();
  ImGuiViewport* vp = ImGui::GetMainViewport();
  const ImVec2 origin = vp->WorkPos;
  const ImVec2 fullSize = vp->WorkSize;

  // Decide proportions (10% toolbar, 25% ctrl panel)
  float toolbarH = fullSize.y * 0.10f;
  float ctrlW = fullSize.x * 0.25f;
  float previewW = fullSize.x - ctrlW;
  float lowerH = fullSize.y - toolbarH;

  PanelRect toolbarRect{origin, {fullSize.x, toolbarH}};
  PanelRect controlRect{{origin.x, origin.y + toolbarH}, {ctrlW, lowerH}};
  PanelRect previewRect{{origin.x + ctrlW, origin.y + toolbarH},
                        {previewW, lowerH}};

  panels::Toolbar::draw(state_, toolbarRect);
  ImGui::Render();
}

}  // namespace psm_gui::ui
