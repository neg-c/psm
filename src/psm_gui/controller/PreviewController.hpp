#pragma once

#include "app/AppState.hpp"
#include <GL/glew.h>

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
};

}  // namespace psm_gui::controller