#include <GLFW/glfw3.h>

#include <iostream>

void glfw_error_callback([[maybe_unused]] int error, const char* desc) {
  std::cout << "Error: " << desc << "\n";
}

void framebuffer_size_callback([[maybe_unused]] GLFWwindow* window, int width,
                               int height) {
  glViewport(0, 0, width, height);
}

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwSetErrorCallback(glfw_error_callback);

  GLFWwindow* window =
      glfwCreateWindow(800, 600, "PSM Gui Demo", nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "Failed to create GLFW window " << "\n";
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  glfwDestroyWindow(window);
  return 0;
}
