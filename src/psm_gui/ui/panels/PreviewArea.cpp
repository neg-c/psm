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

          // Draw magnifying glass
          DrawMagnifyingGlass(s, texture_id, mouse_pos, image_pos, image_size,
                              pixel_x, pixel_y);
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

void PreviewArea::DrawMagnifyingGlass(AppState& s, GLuint texture_id,
                                      const ImVec2& mouse_pos,
                                      const ImVec2& image_pos,
                                      const ImVec2& image_size, int pixel_x,
                                      int pixel_y) {
  // Magnifying glass parameters
  const float radius = 60.0f;
  const float zoom_pixels =
      5.0f;  // Show a 11x11 pixel area (5 on each side of center)

  // Calculate position for magnifying glass (offset from cursor)
  ImVec2 mag_center = ImVec2(mouse_pos.x + radius + 10.0f, mouse_pos.y);

  // Keep magnifying glass within image bounds (simple check)
  if (mag_center.x + radius > image_pos.x + image_size.x) {
    mag_center.x = mouse_pos.x - radius - 10.0f;
  }
  if (mag_center.y + radius > image_pos.y + image_size.y) {
    mag_center.y = mouse_pos.y - radius - 10.0f;
  }

  auto* draw_list = ImGui::GetWindowDrawList();

  // Define the screen area for the magnifying glass
  ImVec2 p_min = ImVec2(mag_center.x - radius, mag_center.y - radius);
  ImVec2 p_max = ImVec2(mag_center.x + radius, mag_center.y + radius);

  // Calculate UV coordinates for the zoomed portion of the texture
  float uv_half_width = (zoom_pixels + 0.5f) / s.io.width;
  float uv_half_height = (zoom_pixels + 0.5f) / s.io.height;
  ImVec2 uv_center =
      ImVec2((pixel_x + 0.5f) / s.io.width, (pixel_y + 0.5f) / s.io.height);
  ImVec2 uv_min =
      ImVec2(uv_center.x - uv_half_width, uv_center.y - uv_half_height);
  ImVec2 uv_max =
      ImVec2(uv_center.x + uv_half_width, uv_center.y + uv_half_height);

  // Draw the magnified image inside a circle
  draw_list->AddImageRounded((void*)(intptr_t)texture_id, p_min, p_max, uv_min,
                             uv_max, IM_COL32_WHITE, radius);

  // Draw the border of the magnifying glass
  draw_list->AddCircle(mag_center, radius, IM_COL32(60, 60, 60, 255), 0, 2.0f);

  // Draw a crosshair/box to highlight the center pixel
  float center_pixel_size = (2 * radius) / (2 * zoom_pixels + 1);
  ImVec2 center_min = ImVec2(mag_center.x - center_pixel_size * 0.5f,
                             mag_center.y - center_pixel_size * 0.5f);
  ImVec2 center_max = ImVec2(mag_center.x + center_pixel_size * 0.5f,
                             mag_center.y + center_pixel_size * 0.5f);
  draw_list->AddRect(center_min, center_max, IM_COL32(255, 0, 0, 255), 0.0f, 0,
                     2.0f);
}

}  // namespace psm_gui::ui::panels
