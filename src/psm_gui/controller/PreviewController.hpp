#pragma once

#include <GL/glew.h>

#include "app/AppState.hpp"

namespace psm_gui::controller {

class PreviewController {
 public:
  explicit PreviewController(AppState& state);
  ~PreviewController();
  GLuint getOrCreateTexture();

 private:
  AppState& state_;
  static GLuint texture_id_;
  static const unsigned char* last_image_update_;
  static int last_width_;
  static int last_height_;
  static bool last_processed_;
  static int last_colorspace_;
};

}  // namespace psm_gui::controller
