#pragma once

#include <string>
#include <vector>

struct AppState {
  struct {
    int width_;
    int height_;
  } size;

  struct {
    std::string load_path_;
    std::string save_path_;
    bool loaded_image = false;
    bool image_processed = false;
    std::vector<unsigned char> original_image;
    std::vector<unsigned char> converted_image;
    std::vector<unsigned char> display_image;
    int width = 0;
    int height = 0;
    int channels = 0;
  } io;

  struct {
    int vertical_slider = 0;
    int horizontal_slider = 0;
  } sliders;

  struct {
    bool valid = false;
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
  } pixel_color;

  int selected_colorspace = 0;
};
