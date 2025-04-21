#pragma once

#include <GLFW/glfw3.h>

#include <string>
#include <vector>

#include "imgui.h"

// Application state
struct AppState {
  // Image data
  GLuint image_texture = 0;
  int image_width = 0;
  int image_height = 0;
  bool has_image = false;

  // Conversion options
  std::vector<std::string> conversion_options = {
      "RGB to Grayscale", "RGB to HSV", "RGB to YCbCr", "Invert Colors",
      "Adjust Brightness"};
  int current_conversion = 0;
  int slider_value = 50;

  // UI state
  ImVec4 clear_color =
      ImVec4(0.118f, 0.169f, 0.161f, 1.00f);  // rgba(30,43,41,255)

  // Add this to your AppState struct/class:
  int vertical_slider_value = 50;  // Default value
};
