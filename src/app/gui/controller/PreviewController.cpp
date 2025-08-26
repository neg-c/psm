#include "PreviewController.hpp"

namespace psm_gui::controller {

GLuint PreviewController::texture_id_ = 0;
const void* PreviewController::last_image_update_ = nullptr;
int PreviewController::last_width_ = 0;
int PreviewController::last_height_ = 0;
bool PreviewController::last_processed_ = false;
int PreviewController::last_colorspace_ = -1;
bool PreviewController::last_is_16bit_ = false;
int PreviewController::update_counter_ = 0;
int PreviewController::last_update_counter_ = 0;

PreviewController::PreviewController(AppState& state) : state_(state) {}

PreviewController::~PreviewController() {
  // We don't delete the texture here since it's static and shared
  // It will be cleaned up when the application exits
}

GLuint PreviewController::getOrCreateTexture() {
  const void* current_image_data = nullptr;

  if (state_.image.is_16bit) {
    const auto& display_data =
        std::get<std::vector<uint16_t>>(state_.image.display_data);
    current_image_data = display_data.data();
  } else {
    const auto& display_data =
        std::get<std::vector<unsigned char>>(state_.image.display_data);
    current_image_data = display_data.data();
  }

  bool image_changed = last_image_update_ != current_image_data;
  bool size_changed =
      last_width_ != state_.image.width || last_height_ != state_.image.height;
  bool processed_changed = last_processed_ != state_.image.is_processed;
  bool colorspace_changed = last_colorspace_ != state_.selected_colorspace;
  bool bit_depth_changed = last_is_16bit_ != state_.image.is_16bit;
  bool force_update = update_counter_ != last_update_counter_;

  if (!texture_id_ || image_changed || size_changed || processed_changed ||
      colorspace_changed || bit_depth_changed || force_update) {
    if (!texture_id_) {
      glGenTextures(1, &texture_id_);
    }

    glBindTexture(GL_TEXTURE_2D, texture_id_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    if (state_.image.is_16bit) {
      // Use 16-bit texture format for 16-bit images
      const auto& display_data =
          std::get<std::vector<uint16_t>>(state_.image.display_data);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, state_.image.width,
                   state_.image.height, 0, GL_RGB, GL_UNSIGNED_SHORT,
                   display_data.data());
    } else {
      // Use 8-bit texture format for 8-bit images
      const auto& display_data =
          std::get<std::vector<unsigned char>>(state_.image.display_data);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, state_.image.width,
                   state_.image.height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                   display_data.data());
    }

    last_image_update_ = current_image_data;
    last_width_ = state_.image.width;
    last_height_ = state_.image.height;
    last_processed_ = state_.image.is_processed;
    last_colorspace_ = state_.selected_colorspace;
    last_is_16bit_ = state_.image.is_16bit;
    last_update_counter_ = update_counter_;
  }

  return texture_id_;
}

}  // namespace psm_gui::controller
