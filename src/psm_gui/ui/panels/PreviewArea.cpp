#include "panels/PreviewArea.hpp"

#include <imgui.h>

#include "PreviewArea.hpp"
#include "controller/PreviewController.hpp"

namespace psm_gui::ui::panels {

void PreviewArea::draw(AppState& s, const PanelRect& r) {
  ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
                           ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
  ImGui::SetNextWindowPos(r.pos);
  ImGui::SetNextWindowSize(r.size);
  ImGui::Begin("PreviewArea", nullptr, flags);

  controller::PreviewController previewCtl(s);

  if (s.io.loaded_image && s.io.image_processed) {
    GLuint texture_id = previewCtl.getOrCreateTexture();

    ImVec2 content_size = ImGui::GetContentRegionAvail();

    ImVec2 image_size;
    image_size.x = content_size.x - 10.0f;
    image_size.y = content_size.y - 10.0f;

    float pos_x = (content_size.x - image_size.x) * 0.5f;
    float pos_y = (content_size.y - image_size.y) * 0.5f;
    ImGui::SetCursorPos(ImVec2(pos_x, pos_y));

    ImGui::Image((void*)(intptr_t)texture_id, image_size);

    ImVec2 mouse_pos = ImGui::GetMousePos();
    ImVec2 window_pos = ImGui::GetWindowPos();
    ImVec2 image_pos = ImVec2(window_pos.x + pos_x, window_pos.y + pos_y);

    if (mouse_pos.x >= image_pos.x &&
        mouse_pos.x < image_pos.x + image_size.x &&
        mouse_pos.y >= image_pos.y &&
        mouse_pos.y < image_pos.y + image_size.y) {
      float rel_x = (mouse_pos.x - image_pos.x) / image_size.x;
      float rel_y = (mouse_pos.y - image_pos.y) / image_size.y;

      int pixel_x = static_cast<int>(rel_x * s.io.width);
      int pixel_y = static_cast<int>(rel_y * s.io.height);

      if (pixel_x >= 0 && pixel_x < s.io.width && pixel_y >= 0 &&
          pixel_y < s.io.height) {
        size_t idx = (pixel_y * s.io.width + pixel_x) * s.io.channels;
        if (idx + 2 < s.io.processed_image.size()) {
          s.pixel_color.valid = true;
          s.pixel_color.r = s.io.processed_image[idx];
          s.pixel_color.g = s.io.processed_image[idx + 1];
          s.pixel_color.b = s.io.processed_image[idx + 2];
        }
      }
    } else {
      s.pixel_color.valid = false;
    }

  } else {
    ImVec2 text_size = ImGui::CalcTextSize("No image loaded");
    float pos_x = (r.size.x - text_size.x) * 0.5f;
    float pos_y = (r.size.y - text_size.y) * 0.5f;
    ImGui::SetCursorPos(ImVec2(pos_x, pos_y));
    ImGui::TextDisabled("No image loaded");
    s.pixel_color.valid = false;
  }

  ImGui::End();
}

}  // namespace psm_gui::ui::panels
