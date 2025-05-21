#include "panels/PreviewArea.hpp"

#include "PreviewArea.hpp"
#include <imgui.h>
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

  } else {
    ImVec2 text_size = ImGui::CalcTextSize("No image loaded");
    float pos_x = (r.size.x - text_size.x) * 0.5f;
    float pos_y = (r.size.y - text_size.y) * 0.5f;
    ImGui::SetCursorPos(ImVec2(pos_x, pos_y));
    ImGui::TextDisabled("No image loaded");
  }

  ImGui::End();
}

}  // namespace psm_gui::ui::panels
