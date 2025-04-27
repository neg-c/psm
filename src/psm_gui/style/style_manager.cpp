#include "style_manager.h"

namespace psm_gui {

// UIStyleManager implementation
UIStyleManager::UIStyleManager(ImGuiCol idx, const ImVec4& col)
    : m_varIdx(ImGuiStyleVar_COUNT),
      m_colIdx(idx),
      m_isColor(true),
      m_isVec2(false) {
  ImGui::PushStyleColor(idx, col);
}

UIStyleManager::UIStyleManager(ImGuiStyleVar idx, float val)
    : m_varIdx(idx),
      m_colIdx(ImGuiCol_COUNT),
      m_isColor(false),
      m_isVec2(false) {
  ImGui::PushStyleVar(idx, val);
}

UIStyleManager::UIStyleManager(ImGuiStyleVar idx, const ImVec2& val)
    : m_varIdx(idx),
      m_colIdx(ImGuiCol_COUNT),
      m_isColor(false),
      m_isVec2(true) {
  ImGui::PushStyleVar(idx, val);
}

UIStyleManager::~UIStyleManager() {
  if (m_isColor) {
    ImGui::PopStyleColor();
  } else {
    ImGui::PopStyleVar();
  }
}

}  // namespace psm_gui
