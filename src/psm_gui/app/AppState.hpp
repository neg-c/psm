#pragma once

#include <string>

struct AppState {
  struct {
    int width_;
    int height_;
  } size;

  struct {
    std::string load_path_;
    std::string save_path_;
  } io;
};
