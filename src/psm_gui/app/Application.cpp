#include "Application.hpp"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>
#include <memory>

#include "AppState.hpp"
#include "UIRenderer.hpp"

namespace psm_gui {

Application::Application() {
  // GL 3.0 + GLSL 130
  const char* glsl_version = "#version 130";
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwSetErrorCallback(error_callback);

  GLFWmonitor* primary = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(primary);
  int screenW = mode->width;
  int screenH = mode->height;

  constexpr float target_width = 1024.0f / 1920.0f;
  constexpr float target_height = 768.0f / 1080.0f;
  int winW = static_cast<int>(screenW * target_width + 0.5f);
  int winH = static_cast<int>(screenH * target_height + 0.5f);

  state_ = AppState{winW, winH, 4};

  window_ = glfwCreateWindow(state_.size.width_, state_.size.height_,
                             "PSM Gui Demo", nullptr, nullptr);
  if (window_ == nullptr) {
    std::cout << "Failed to create GLFW window " << "\n";
    glfwTerminate();
  }

  glfwSetWindowPos(window_, screenW / 5, screenH / 7);
  glfwMakeContextCurrent(window_);
  glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.IniFilename = nullptr;

  ImGui_ImplGlfw_InitForOpenGL(window_, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
}

Application::~Application() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window_);
}

int Application::run() {
  auto renderer_ = std::make_unique<ui::UIRenderer>(state_);
  while (!glfwWindowShouldClose(window_)) {
    glfwPollEvents();
    glfwSwapBuffers(window_);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    renderer_->render();

    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }
  return 0;
}

void Application::error_callback([[maybe_unused]] int error, const char* desc) {
  std::cout << "Error: " << desc << "\n";
}

void Application::framebuffer_size_callback([[maybe_unused]] GLFWwindow* window,
                                            int width, int height) {
  glViewport(0, 0, width, height);
}
};  // namespace psm_gui
