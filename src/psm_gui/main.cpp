#include <GLFW/glfw3.h>

#include <iostream>

#include "app/app_state.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ui/ui_renderer.h"
#include "utils/file_dialog.h"
#include "utils/image_loader.h"
#include "utils/image_processor.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && \
    !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// GLFW error callback
static void glfw_error_callback(int error, const char* description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static void calculate_window_size(int& width, int& height) {
  GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();
  if (primary_monitor) {
    const GLFWvidmode* mode = glfwGetVideoMode(primary_monitor);
    if (mode) {
      // Calculate size to be 60% of the monitor's height while maintaining 4:3 ratio
      height = static_cast<int>(mode->height * 0.6);
      width = static_cast<int>(height * 4.0 / 3.0);

      // If width is too large for the monitor, scale down based on width instead
      if (width > mode->width * 0.6) {
        width = static_cast<int>(mode->width * 0.6);
        height = static_cast<int>(width * 3.0 / 4.0);
      }
    }
  }
}

// Main code
int main(int, char**) {
  // Setup error callback
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) return 1;

  // GL 3.0 + GLSL 130
  const char* glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  int window_width = 1280;
  int window_height = 960;
  calculate_window_size(window_width, window_height);

  GLFWwindow* window = glfwCreateWindow(window_width, window_height,
                                        "PSM GUI Demo", nullptr, nullptr);
  if (window == nullptr) return 1;
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);  // Enable vsync

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.IniFilename = nullptr;

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Initialize application state
  psm_gui::AppState state;

  // Create UI renderer
  psm_gui::UIRenderer uiRenderer(state);

  // Main loop
  while (!glfwWindowShouldClose(window)) {
    // Poll and handle events
    glfwPollEvents();
    if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
      continue;
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Render the main UI using the UI renderer
    uiRenderer.render();

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(state.clear_color.x * state.clear_color.w,
                 state.clear_color.y * state.clear_color.w,
                 state.clear_color.z * state.clear_color.w,
                 state.clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  // Cleanup
  if (state.image_texture) {
    glDeleteTextures(1, &state.image_texture);
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
