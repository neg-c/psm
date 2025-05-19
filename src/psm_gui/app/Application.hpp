#pragma once

#include <GLFW/glfw3.h>

#include "Actions.hpp"
#include "AppState.hpp"

namespace psm_gui {
class Application {
 public:
  Application();
  ~Application();
  int run();

 private:
  GLFWwindow* window_;
  AppState state_;
  Actions actions_;
  static void error_callback([[maybe_unused]] int error, const char* desc);
  static void framebuffer_size_callback([[maybe_unused]] GLFWwindow* window,
                                        int width, int height);
};

}  // namespace psm_gui
