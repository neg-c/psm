#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace psm_gui {

/**
 * @brief Application state management
 *
 * Simple, focused state management for the PSM GUI application.
 * Organized into logical groups for better readability.
 */
struct AppState {
  // Window dimensions
  struct WindowSize {
    int width = 0;
    int height = 0;

    bool isValid() const { return width > 0 && height > 0; }
  } window;

  // Image data and file operations
  struct ImageData {
    std::string load_path;
    std::string save_path;
    bool is_loaded = false;
    bool is_processed = false;

    // Use 16-bit buffers for all image data (8-bit data stored in lower range)
    std::vector<std::uint16_t> original_data;
    std::vector<std::uint16_t> converted_data;
    std::vector<std::uint16_t> display_data;

    // Track original bit depth for proper display conversion
    bool is_original_8bit = true;

    int width = 0;
    int height = 0;
    int channels = 3;  // Default to RGB

    bool hasValidImage() const {
      return is_loaded && width > 0 && height > 0 && !original_data.empty();
    }

    size_t getImageSize() const {
      return static_cast<size_t>(width) * height * channels;
    }

    void clear() {
      load_path.clear();
      save_path.clear();
      is_loaded = false;
      is_processed = false;
      original_data.clear();
      converted_data.clear();
      display_data.clear();
      width = 0;
      height = 0;
      channels = 3;
      is_original_8bit = true;
    }
  } image;

  // UI control state
  struct Controls {
    int vertical_slider = 0;
    int horizontal_slider = 0;
    std::string vertical_label = "Brightness";
    std::string horizontal_label = "Contrast";

    void resetSliders() {
      vertical_slider = 0;
      horizontal_slider = 0;
    }
  } controls;

  // Pixel information for display
  struct PixelInfo {
    bool is_valid = false;
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;

    void setColor(unsigned char red, unsigned char green, unsigned char blue) {
      r = red;
      g = green;
      b = blue;
      is_valid = true;
    }

    void clear() {
      is_valid = false;
      r = g = b = 0;
    }
  } pixel;

  // Application settings
  int selected_colorspace = 0;

  // Constructor for proper initialization
  AppState() = default;
  explicit AppState(int window_width, int window_height)
      : window{window_width, window_height} {}

  // Simple validation
  bool isValid() const { return window.isValid(); }

  // Reset to initial state
  void reset() {
    window = WindowSize{};
    image.clear();
    controls.resetSliders();
    pixel.clear();
    selected_colorspace = 0;
  }
};

}  // namespace psm_gui
