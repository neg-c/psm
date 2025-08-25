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

  if (s.image.is_loaded && s.image.is_processed) {
    GLuint texture_id = previewCtl.getOrCreateTexture();

    ImVec2 content_size = ImGui::GetContentRegionAvail();
    ImVec2 available_size;
    available_size.x = content_size.x - 10.0f;
    available_size.y = content_size.y - 10.0f;

    ImVec2 image_size;
    float image_aspect_ratio =
        static_cast<float>(s.image.width) / static_cast<float>(s.image.height);
    float available_aspect_ratio = available_size.x / available_size.y;

    if (image_aspect_ratio > available_aspect_ratio) {
      // Image is wider than available space, fit to width
      image_size.x = available_size.x;
      image_size.y = available_size.x / image_aspect_ratio;
    } else {
      // Image is taller than available space, fit to height
      image_size.y = available_size.y;
      image_size.x = available_size.y * image_aspect_ratio;
    }
    // Center the image
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

      int pixel_x = static_cast<int>(rel_x * s.image.width);
      int pixel_y = static_cast<int>(rel_y * s.image.height);

      if (pixel_x >= 0 && pixel_x < s.image.width && pixel_y >= 0 &&
          pixel_y < s.image.height) {
        size_t idx = (pixel_y * s.image.width + pixel_x) * s.image.channels;
        if (idx + 2 < s.image.display_data.size()) {
          s.pixel.setColor(s.image.display_data[idx],
                           s.image.display_data[idx + 1],
                           s.image.display_data[idx + 2]);

          // Draw magnifying glass
          DrawMagnifyingGlass(s, texture_id, mouse_pos, image_pos, image_size,
                              pixel_x, pixel_y);
        }
      }
    } else {
      s.pixel.clear();
    }

  } else {
    ImVec2 text_size = ImGui::CalcTextSize("No image loaded");
    float pos_x = (r.size.x - text_size.x) * 0.5f;
    float pos_y = (r.size.y - text_size.y) * 0.5f;
    ImGui::SetCursorPos(ImVec2(pos_x, pos_y));
    ImGui::TextDisabled("No image loaded");
    s.pixel.clear();
  }

  ImGui::End();
}

void PreviewArea::DrawMagnifyingGlass(AppState& s, GLuint texture_id,
                                      const ImVec2& mouse_pos,
                                      const ImVec2& image_pos,
                                      const ImVec2& image_size, int pixel_x,
                                      int pixel_y) {
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

  ImVec2 p_min = ImVec2(mag_center.x - radius, mag_center.y - radius);
  ImVec2 p_max = ImVec2(mag_center.x + radius, mag_center.y + radius);

  float uv_half_width = (zoom_pixels + 0.5f) / s.image.width;
  float uv_half_height = (zoom_pixels + 0.5f) / s.image.height;
  ImVec2 uv_center = ImVec2((pixel_x + 0.5f) / s.image.width,
                            (pixel_y + 0.5f) / s.image.height);
  ImVec2 uv_min =
      ImVec2(uv_center.x - uv_half_width, uv_center.y - uv_half_height);
  ImVec2 uv_max =
      ImVec2(uv_center.x + uv_half_width, uv_center.y + uv_half_height);

  draw_list->AddImageRounded((void*)(intptr_t)texture_id, p_min, p_max, uv_min,
                             uv_max, IM_COL32_WHITE, radius);

  draw_list->AddCircle(mag_center, radius, IM_COL32(60, 60, 60, 255), 0, 2.0f);

  float center_pixel_size = (2 * radius) / (2 * zoom_pixels + 1);
  ImVec2 center_min = ImVec2(mag_center.x - (center_pixel_size * 0.5f),
                             mag_center.y - (center_pixel_size * 0.5f));
  ImVec2 center_max = ImVec2(mag_center.x + (center_pixel_size * 0.5f),
                             mag_center.y + (center_pixel_size * 0.5f));
  draw_list->AddRect(center_min, center_max, IM_COL32(255, 0, 0, 255), 0.0f, 0,
                     2.0f);
}

}  // namespace psm_gui::ui::panels
