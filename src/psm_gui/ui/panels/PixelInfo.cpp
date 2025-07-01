#include "PixelInfo.hpp"

#include <imgui.h>

#include <iomanip>
#include <sstream>
#include <string>

namespace psm_gui::ui::panels {

void PixelInfo::draw(AppState& s, const PanelRect& r) {
  ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
                           ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
  ImGui::SetNextWindowPos(r.pos);
  ImGui::SetNextWindowSize(r.size);
  ImGui::Begin("PixelInfo", nullptr, flags);

  if (s.pixel.is_valid) {
    ImVec2 color_size(r.size.x * 0.8f, r.size.y * 0.6f);
    ImVec2 center_pos = ImVec2(r.size.x * 0.5f - color_size.x * 0.5f, 20);
    ImGui::SetCursorPos(center_pos);

    ImVec4 color = ImVec4(s.pixel.r / 255.0f, s.pixel.g / 255.0f,
                          s.pixel.b / 255.0f, 1.0f);

    ImGui::GetWindowDrawList()->AddRectFilled(
        ImGui::GetCursorScreenPos(),
        ImVec2(ImGui::GetCursorScreenPos().x + color_size.x,
               ImGui::GetCursorScreenPos().y + color_size.y),
        ImGui::ColorConvertFloat4ToU32(color),
        10.0f  // Rounding
    );

    std::stringstream ss;
    ss << "RGB:(" << static_cast<int>(s.pixel.r) << ","
       << static_cast<int>(s.pixel.g) << ","
       << static_cast<int>(s.pixel.b) << ")";

    std::string rgb_text = ss.str();
    ImVec2 text_size = ImGui::CalcTextSize(rgb_text.c_str());
    ImVec2 text_pos = ImVec2(
        r.size.x * 0.5f - text_size.x * 0.5f,  // Center text horizontally
        center_pos.y + color_size.y + 10);
    ImGui::SetCursorPos(text_pos);
    ImGui::Text("%s", rgb_text.c_str());
  } else {
    ImVec2 text_size = ImGui::CalcTextSize("No pixel selected");
    float pos_x = (r.size.x - text_size.x) * 0.5f;
    float pos_y = (r.size.y - text_size.y) * 0.5f;
    ImGui::SetCursorPos(ImVec2(pos_x, pos_y));
    ImGui::TextDisabled("No pixel selected");
  }

  ImGui::End();
}

}  // namespace psm_gui::ui::panels
