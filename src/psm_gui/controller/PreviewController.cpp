#include "PreviewController.hpp"

#include <cstdint>
#include <vector>

namespace psm_gui::controller {

GLuint PreviewController::texture_id_ = 0;
const unsigned char* PreviewController::last_image_update_ = nullptr;
int PreviewController::last_width_ = 0;
int PreviewController::last_height_ = 0;
bool PreviewController::last_processed_ = false;
int PreviewController::last_colorspace_ = -1;
int PreviewController::update_counter_ = 0;
int PreviewController::last_update_counter_ = 0;

PreviewController::PreviewController(AppState& state) : state_(state) {}

PreviewController::~PreviewController() {
  // We don't delete the texture here since it's static and shared
  // It will be cleaned up when the application exits
}

GLuint PreviewController::getOrCreateTexture() {
  // Get the current display data pointer
  const void* current_data = nullptr;
  if (state_.image.bit_depth == AppState::ImageData::BitDepth::BITS_16) {
    if (std::holds_alternative<std::vector<std::uint16_t>>(
            state_.image.display_data)) {
      current_data =
          std::get<std::vector<std::uint16_t>>(state_.image.display_data)
              .data();
    }
  } else {
    if (std::holds_alternative<std::vector<std::uint8_t>>(
            state_.image.display_data)) {
      current_data =
          std::get<std::vector<std::uint8_t>>(state_.image.display_data).data();
    }
  }

  bool image_changed =
      last_image_update_ != static_cast<const unsigned char*>(current_data);
  bool size_changed =
      last_width_ != state_.image.width || last_height_ != state_.image.height;
  bool processed_changed = last_processed_ != state_.image.is_processed;
  bool colorspace_changed = last_colorspace_ != state_.selected_colorspace;
  bool force_update = update_counter_ != last_update_counter_;

  if (!texture_id_ || image_changed || size_changed || processed_changed ||
      colorspace_changed || force_update) {
    if (!texture_id_) {
      glGenTextures(1, &texture_id_);
    }

    glBindTexture(GL_TEXTURE_2D, texture_id_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Handle different bit depths
    if (state_.image.bit_depth == AppState::ImageData::BitDepth::BITS_16) {
      // Convert 16-bit data to 8-bit for OpenGL display
      if (std::holds_alternative<std::vector<std::uint16_t>>(
              state_.image.display_data)) {
        const auto& display_data_16 =
            std::get<std::vector<std::uint16_t>>(state_.image.display_data);
        std::vector<std::uint8_t> display_data_8(display_data_16.size());

        for (size_t i = 0; i < display_data_16.size(); ++i) {
          display_data_8[i] =
              static_cast<std::uint8_t>(display_data_16[i] >> 8);
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, state_.image.width,
                     state_.image.height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                     display_data_8.data());
      }
    } else {
      // 8-bit data can be used directly
      if (std::holds_alternative<std::vector<std::uint8_t>>(
              state_.image.display_data)) {
        const auto& display_data_8 =
            std::get<std::vector<std::uint8_t>>(state_.image.display_data);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, state_.image.width,
                     state_.image.height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                     display_data_8.data());
      }
    }

    last_image_update_ = static_cast<const unsigned char*>(current_data);
    last_width_ = state_.image.width;
    last_height_ = state_.image.height;
    last_processed_ = state_.image.is_processed;
    last_colorspace_ = state_.selected_colorspace;
    last_update_counter_ = update_counter_;
  }

  return texture_id_;
}

}  // namespace psm_gui::controller
