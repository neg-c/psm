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
    ImVec2 color_size(r.size.x * 0.8f,
                      r.size.y * 0.4f);  // Reduced height to make room for text
    ImVec2 center_pos = ImVec2(r.size.x * 0.5f - color_size.x * 0.5f, 10);
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

    // Show 8-bit RGB values
    std::stringstream ss;
    ss << "RGB:(" << static_cast<int>(s.pixel.r) << ","
       << static_cast<int>(s.pixel.g) << "," << static_cast<int>(s.pixel.b)
       << ")";

    std::string rgb_text = ss.str();
    ImVec2 text_size = ImGui::CalcTextSize(rgb_text.c_str());
    ImVec2 text_pos = ImVec2(
        r.size.x * 0.5f - text_size.x * 0.5f,  // Center text horizontally
        center_pos.y + color_size.y + 5);
    ImGui::SetCursorPos(text_pos);
    ImGui::Text("%s", rgb_text.c_str());

    // If this is a 16-bit image, also show the original 16-bit values
    if (s.image.is_16bit && s.pixel.is_valid) {
      // Show the actual stored 16-bit values
      std::stringstream ss16;
      ss16 << "16-bit: (" << static_cast<int>(s.pixel.r_16bit) << ","
           << static_cast<int>(s.pixel.g_16bit) << ","
           << static_cast<int>(s.pixel.b_16bit) << ")";

      std::string rgb16_text = ss16.str();
      ImVec2 text16_size = ImGui::CalcTextSize(rgb16_text.c_str());
      ImVec2 text16_pos = ImVec2(r.size.x * 0.5f - text16_size.x * 0.5f,
                                 text_pos.y + text_size.y + 5);
      ImGui::SetCursorPos(text16_pos);
      ImGui::TextDisabled("%s", rgb16_text.c_str());

      // Show pixel coordinates
      std::stringstream ss_coord;
      ss_coord << "Pixel: (" << s.pixel.x << ", " << s.pixel.y << ")";
      std::string coord_text = ss_coord.str();
      ImVec2 coord_size = ImGui::CalcTextSize(coord_text.c_str());
      ImVec2 coord_pos = ImVec2(r.size.x * 0.5f - coord_size.x * 0.5f,
                                text16_pos.y + text16_size.y + 5);
      ImGui::SetCursorPos(coord_pos);
      ImGui::TextDisabled("%s", coord_text.c_str());
    }
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
