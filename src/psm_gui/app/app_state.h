#pragma once

#include <GLFW/glfw3.h>

#include "conversion_types.h"
#include "imgui.h"

namespace psm_gui {

// Application state
struct AppState {
  // Image data
  GLuint image_texture = 0;
  int image_width = 0;
  int image_height = 0;
  bool has_image = false;

  // Conversion state
  int current_conversion = 0;
  int slider_value = 50;
  int vertical_slider_value = 50;  // Default value

  // UI state
  ImVec4 clear_color =
      ImVec4(0.118f, 0.169f, 0.161f, 1.00f);  // rgba(30,43,41,255)

  // Conversion registry
  ConversionRegistry conversion_registry;

  // Slider values for different color spaces
  struct {
    int chromaticity = 50;
    int coolWarm = 50;
  } oRGB_sliders;

  struct {
    int brightness = 50;
    int contrast = 50;
  } sRGB_sliders;

  struct {
    int saturation = 50;
    int tone = 50;
  } adobeRGB_sliders;

  struct {
    int value1 = 50;
    int value2 = 50;
  } displayP3_sliders;

  struct {
    int value1 = 50;
    int value2 = 50;
  } proPhotoRGB_sliders;

  // Before/after divider position (0-100%)
  int beforeAfterDivider = 50;

  // Image metadata
  std::string fileName;
  float fileSizeMB = 0.0f;

  // Helper method to get current conversion type
  ConversionType getCurrentConversionType() const {
    return conversion_registry.getConversionType(current_conversion);
  }

  // Helper method to get current conversion name
  const std::string& getCurrentConversionName() const {
    return conversion_registry.getConversionName(current_conversion);
  }
};

}  // namespace psm_gui
