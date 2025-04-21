#include <GLFW/glfw3.h>
#include <stdio.h>

#include "app_state.h"
#include "image_loader.h"
#include "image_processor.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ui_renderer.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && \
    !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// GLFW error callback
static void glfw_error_callback(int error, const char* description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Add a function to handle the conversion button click
void HandleConversion(psm_gui::AppState& state) {
  if (!state.has_image || state.image_texture == 0) {
    return;
  }

  // Get the image data from the texture
  int width = state.image_width;
  int height = state.image_height;
  std::vector<unsigned char> imageData(width * height * 3);

  // Bind the texture to read its data
  glBindTexture(GL_TEXTURE_2D, state.image_texture);
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData.data());

  // Process the image
  if (psm_gui::ImageProcessor::ProcessImage(state, imageData, width, height,
                                            3)) {
    // Update the texture with the processed image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, imageData.data());
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

  // Create window with graphics context
  GLFWwindow* window =
      glfwCreateWindow(800, 610, "PSM GUI Demo", nullptr, nullptr);
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
