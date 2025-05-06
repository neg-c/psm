#include "Application.hpp"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>

namespace psm_gui {

Application::Application() {
  // GL 3.0 + GLSL 130
  const char* glsl_version = "#version 130";
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwSetErrorCallback(error_callback);
  window_ = glfwCreateWindow(800, 600, "PSM Gui Demo", nullptr, nullptr);
  if (window_ == nullptr) {
    std::cout << "Failed to create GLFW window " << "\n";
    glfwTerminate();
  }
  glfwMakeContextCurrent(window_);
  glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
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
  while (!glfwWindowShouldClose(window_)) {
    glfwPollEvents();
    glfwSwapBuffers(window_);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow();
    ImGui::Render();
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
