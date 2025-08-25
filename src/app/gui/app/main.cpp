#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Application.hpp"

int main() {
  psm_gui::Application app;
  app.run();

  return 0;
}
