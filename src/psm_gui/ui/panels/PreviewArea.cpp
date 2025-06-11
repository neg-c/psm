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
          DrawMagnifyingGlass(s, mouse_pos, image_pos, image_size, pixel_x, pixel_y);
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

void PreviewArea::DrawMagnifyingGlass(
    AppState& s, const ImVec2& mouse_pos, const ImVec2& image_pos, 
    const ImVec2& image_size, int pixel_x, int pixel_y) {
  
  // Magnifying glass parameters
  const float radius = 60.0f;
  const int zoom_factor = 8;
  const int grid_size = 5; // How many pixels to show in each direction
  
  // Calculate position for magnifying glass (offset from cursor)
  ImVec2 mag_center = ImVec2(mouse_pos.x + radius + 10.0f, mouse_pos.y);
  
  // Keep magnifying glass within image bounds
  if (mag_center.x + radius > image_pos.x + image_size.x) {
    mag_center.x = mouse_pos.x - radius - 10.0f;
  }
  if (mag_center.y + radius > image_pos.y + image_size.y) {
    mag_center.y = mouse_pos.y - radius - 10.0f;
  }
  
  auto* draw_list = ImGui::GetWindowDrawList();
  
  // Draw circle background
  draw_list->AddCircleFilled(mag_center, radius, IM_COL32(240, 240, 240, 240));
  draw_list->AddCircle(mag_center, radius, IM_COL32(60, 60, 60, 255), 0, 2.0f);
  
  // Calculate cell size for the grid
  float cell_size = (radius * 2.0f) / (grid_size * 2 + 1);
  
  // Calculate top-left corner of the grid
  ImVec2 grid_start = ImVec2(
    mag_center.x - cell_size * grid_size,
    mag_center.y - cell_size * grid_size
  );
  
  // Draw pixel grid
  for (int y = -grid_size; y <= grid_size; y++) {
    for (int x = -grid_size; x <= grid_size; x++) {
      int img_x = pixel_x + x;
      int img_y = pixel_y + y;
      
      // Skip pixels outside image bounds
      if (img_x < 0 || img_x >= s.io.width || img_y < 0 || img_y >= s.io.height) {
        continue;
      }
      
      // Calculate pixel color from image data
      size_t idx = (img_y * s.io.width + img_x) * s.io.channels;
      if (idx + 2 < s.io.processed_image.size()) {
        ImVec2 pixel_pos = ImVec2(
          grid_start.x + (x + grid_size) * cell_size,
          grid_start.y + (y + grid_size) * cell_size
        );
        
        // Draw pixel as a filled square
        ImU32 pixel_color = IM_COL32(
          s.io.processed_image[idx],
          s.io.processed_image[idx + 1],
          s.io.processed_image[idx + 2],
          255
        );
        
        draw_list->AddRectFilled(
          pixel_pos,
          ImVec2(pixel_pos.x + cell_size, pixel_pos.y + cell_size),
          pixel_color
        );
        
        // Draw grid lines
        draw_list->AddRect(
          pixel_pos,
          ImVec2(pixel_pos.x + cell_size, pixel_pos.y + cell_size),
          IM_COL32(100, 100, 100, 150),
          0.0f,
          0,
          1.0f
        );
        
        // Highlight current pixel
        if (x == 0 && y == 0) {
          draw_list->AddRect(
            pixel_pos,
            ImVec2(pixel_pos.x + cell_size, pixel_pos.y + cell_size),
            IM_COL32(255, 0, 0, 255),
            0.0f,
            0,
            2.0f
          );
        }
      }
    }
  }
}

}  // namespace psm_gui::ui::panels
