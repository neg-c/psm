#pragma once

#include <GLFW/glfw3.h>

namespace psm_gui {

// Helper function to load an image into a OpenGL texture with stb_image.h
bool LoadTextureFromFile(const char* filename, GLuint* out_texture,
                         int* out_width, int* out_height);

}  // namespace psm_gui
