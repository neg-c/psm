#pragma once

#include <cstdint>
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

    // Support for both 8-bit and 16-bit image data
    enum class BitDepth { BITS_8 = 8, BITS_16 = 16 };
    BitDepth bit_depth = BitDepth::BITS_8;

    // Use variant to store different data types
    std::variant<std::vector<std::uint8_t>, std::vector<std::uint16_t>>
        original_data;
    std::variant<std::vector<std::uint8_t>, std::vector<std::uint16_t>>
        converted_data;
    std::variant<std::vector<std::uint8_t>, std::vector<std::uint16_t>>
        display_data;

    int width = 0;
    int height = 0;
    int channels = 3;  // Default to RGB

    bool hasValidImage() const {
      return is_loaded && width > 0 && height > 0 &&
             (std::holds_alternative<std::vector<std::uint8_t>>(original_data) ||
              std::holds_alternative<std::vector<std::uint16_t>>(original_data));
    }

    size_t getImageSize() const {
      return static_cast<size_t>(width) * height * channels;
    }

    void clear() {
      load_path.clear();
      save_path.clear();
      is_loaded = false;
      is_processed = false;
      original_data = std::vector<std::uint8_t>{};
      converted_data = std::vector<std::uint8_t>{};
      display_data = std::vector<std::uint8_t>{};
      width = 0;
      height = 0;
      channels = 3;
      bit_depth = BitDepth::BITS_8;
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
