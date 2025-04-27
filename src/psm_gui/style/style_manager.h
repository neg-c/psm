#pragma once

#include "imgui.h"

namespace psm_gui {

/**
 * @brief Manages ImGui style states using RAII
 */
class UIStyleManager {
 public:
  /**
   * @brief Push a style color
   *
   * @param idx Style color index
   * @param col Color value
   */
  UIStyleManager(ImGuiCol idx, const ImVec4& col);

  /**
   * @brief Push a style variable
   *
   * @param idx Style variable index
   * @param val Style variable value
   */
  UIStyleManager(ImGuiStyleVar idx, float val);

  /**
   * @brief Push a style variable with 2D vector value
   *
   * @param idx Style variable index
   * @param val Style variable value
   */
  UIStyleManager(ImGuiStyleVar idx, const ImVec2& val);

  /**
   * @brief Destroy the UI Style Manager and pop the style
   */
  ~UIStyleManager();

 private:
  ImGuiStyleVar m_varIdx;
  ImGuiCol m_colIdx;
  bool m_isColor;
  bool m_isVec2;
};

}  // namespace psm_gui
