#include "UIRenderer.hpp"

#include <imgui_internal.h>

#include "panels/HorizontalSlider.hpp"
#include "panels/InfoPanel.hpp"
#include "panels/PreviewArea.hpp"
#include "panels/Toolbar.hpp"
#include "panels/VerticalSlider.hpp"

namespace psm_gui::ui {
UIRenderer::UIRenderer(AppState& state) : state_(state) {}

void UIRenderer::render() {
  ImGui::NewFrame();

  ImGuiViewport* vp = ImGui::GetMainViewport();
  const ImVec2 origin = vp->WorkPos;
  const ImVec2 sizeAll = vp->WorkSize;

  float toolbarH = sizeAll.y * 0.10f;  // 10% for toolbar
  float contentH = sizeAll.y - toolbarH;

  constexpr float rowSplit = 0.80f;  // first row = 60%, second = 40%
  float firstRowH = contentH * rowSplit;
  float secondRowH = contentH - firstRowH;

  constexpr float colSplit = 0.20f;  // first col = 60%, second = 40%
  float firstColW = sizeAll.x * colSplit;
  float secondColW = sizeAll.x - firstColW;

  PanelRect toolbar{origin, {sizeAll.x, toolbarH}};
  PanelRect satPanel{{origin.x, origin.y + toolbarH}, {firstColW, firstRowH}};
  PanelRect preview{{origin.x + firstColW, origin.y + toolbarH},
                    {secondColW, firstRowH}};
  PanelRect infoPanel{{origin.x, origin.y + toolbarH + firstRowH},
                      {firstColW, secondRowH}};
  PanelRect tonePanel{{origin.x + firstColW, origin.y + toolbarH + firstRowH},
                      {secondColW, secondRowH}};

  panels::Toolbar::draw(state_, toolbar);
  panels::VerticalSlider::draw(state_, satPanel);
  panels::PreviewArea::draw(state_, preview);
  panels::InfoPanel::draw(state_, infoPanel);
  panels::HorizontalSlider::draw(state_, tonePanel);

  ImGui::Render();
}

}  // namespace psm_gui::ui
