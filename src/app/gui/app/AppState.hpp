#pragma once

#include <cstdint>
#include <span>
#include <string>
#include <variant>
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
    
    // Support both 8-bit and 16-bit image data
    using ImageVariant = std::variant<std::vector<unsigned char>, std::vector<uint16_t>>;
    ImageVariant original_data;
    ImageVariant converted_data;
    ImageVariant display_data;
    
    int width = 0;
    int height = 0;
    int channels = 3;  // Default to RGB
    bool is_16bit = false;  // Track if we're working with 16-bit data

    bool hasValidImage() const {
      return is_loaded && width > 0 && height > 0 && 
             std::visit([](const auto& data) { return !data.empty(); }, original_data);
    }

    size_t getImageSize() const {
      return static_cast<size_t>(width) * height * channels;
    }

    // Helper functions to get data size
    size_t getOriginalDataSize() const {
      return std::visit([](const auto& data) { return data.size(); }, original_data);
    }

    size_t getConvertedDataSize() const {
      return std::visit([](const auto& data) { return data.size(); }, converted_data);
    }

    size_t getDisplayDataSize() const {
      return std::visit([](const auto& data) { return data.size(); }, display_data);
    }

    void clear() {
      load_path.clear();
      save_path.clear();
      is_loaded = false;
      is_processed = false;
      original_data = std::vector<unsigned char>{};
      converted_data = std::vector<unsigned char>{};
      display_data = std::vector<unsigned char>{};
      width = 0;
      height = 0;
      channels = 3;
      is_16bit = false;
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
    uint16_t r_16bit = 0;  // Store original 16-bit values
    uint16_t g_16bit = 0;
    uint16_t b_16bit = 0;
    int x = 0;  // Pixel coordinates
    int y = 0;

    void setColor(unsigned char red, unsigned char green, unsigned char blue) {
      r = red;
      g = green;
      b = blue;
      is_valid = true;
    }

    void setColor16Bit(unsigned char red, unsigned char green, unsigned char blue,
                       uint16_t red_16, uint16_t green_16, uint16_t blue_16,
                       int pixel_x, int pixel_y) {
      r = red;
      g = green;
      b = blue;
      r_16bit = red_16;
      g_16bit = green_16;
      b_16bit = blue_16;
      x = pixel_x;
      y = pixel_y;
      is_valid = true;
    }

    void clear() {
      is_valid = false;
      r = g = b = 0;
      r_16bit = g_16bit = b_16bit = 0;
      x = y = 0;
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
