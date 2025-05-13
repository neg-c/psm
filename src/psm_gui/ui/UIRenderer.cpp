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

  constexpr float rounding = 8.0f;
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, rounding);

  ImGuiViewport* vp = ImGui::GetMainViewport();
  const ImVec2 origin = vp->WorkPos;
  const ImVec2 full = vp->WorkSize;

  constexpr float pad = 10.0f;
  const float innerX = origin.x + pad;
  const float innerY = origin.y + pad;
  const float innerW = full.x - (2 * pad);
  const float innerH = full.y - (2 * pad);

  const float toolbarH = innerH * 0.10f;
  const float contentH = innerH - toolbarH - pad;
  constexpr float rowSplit = 0.80f;
  const float firstRowH = (contentH * rowSplit) - pad;
  const float secondRowH = contentH - firstRowH - pad;
  constexpr float colSplit = 0.20f;
  const float availW = innerW - pad;
  const float firstColW = availW * colSplit;
  const float secondColW = availW - firstColW;

  PanelRect toolbar{{innerX, innerY}, {innerW, toolbarH}};
  const float row0Y = innerY + toolbarH + pad;
  PanelRect satPanel{{innerX, row0Y}, {firstColW, firstRowH}};
  PanelRect preview{{innerX + firstColW + pad, row0Y}, {secondColW, firstRowH}};
  const float row1Y = row0Y + firstRowH + pad;
  PanelRect infoPanel{{innerX, row1Y}, {firstColW, secondRowH}};
  PanelRect tonePanel{{innerX + firstColW + pad, row1Y},
                      {secondColW, secondRowH}};

  panels::Toolbar::draw(state_, toolbar);
  panels::VerticalSlider::draw(state_, satPanel);
  panels::PreviewArea::draw(state_, preview);
  panels::InfoPanel::draw(state_, infoPanel);
  panels::HorizontalSlider::draw(state_, tonePanel);

  ImGui::Render();
}

}  // namespace psm_gui::ui
