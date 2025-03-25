#include <GLFW/glfw3.h>
#include <stdio.h>

#include <filesystem>
#include <string>
#include <vector>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "stb_image.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && \
    !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// Application state
struct AppState {
  // Image data
  GLuint image_texture = 0;
  int image_width = 0;
  int image_height = 0;
  bool has_image = false;

  // Conversion options
  std::vector<std::string> conversion_options = {
      "RGB to Grayscale", "RGB to HSV", "RGB to YCbCr", "Invert Colors",
      "Adjust Brightness"};
  int current_conversion = 0;
  int slider_value = 50;

  // UI state
  ImVec4 clear_color =
      ImVec4(0.118f, 0.169f, 0.161f, 1.00f);  // rgba(30,43,41,255)
};

// Forward declarations
static void glfw_error_callback(int error, const char* description);
bool LoadTextureFromFile(const char* filename, GLuint* out_texture,
                         int* out_width, int* out_height);
void RenderPreviewArea(AppState& state, float width, float height);
void RenderControlsArea(AppState& state, float window_width,
                        float window_height);

// Helper function to load an image into a OpenGL texture with stb_image.h
bool LoadTextureFromFile(const char* filename, GLuint* out_texture,
                         int* out_width, int* out_height) {
  // Load from file
  int image_width = 0;
  int image_height = 0;
  unsigned char* image_data =
      stbi_load(filename, &image_width, &image_height, NULL, 4);
  if (image_data == NULL) return false;

  // Create a OpenGL texture identifier
  GLuint image_texture;
  glGenTextures(1, &image_texture);
  glBindTexture(GL_TEXTURE_2D, image_texture);

  // Setup filtering parameters for display
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, image_data);
  stbi_image_free(image_data);

  *out_texture = image_texture;
  *out_width = image_width;
  *out_height = image_height;

  return true;
}

// GLFW error callback
static void glfw_error_callback(int error, const char* description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Render the preview area with the image or placeholder
void RenderPreviewArea(AppState& state, float width, float height) {
  // Style the preview area
  ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.184f, 0.184f, 0.184f, 1.0f));
  ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 15.0f);

  ImGui::BeginChild("Preview", ImVec2(width, height), true);

  if (state.has_image && state.image_texture) {
    // Calculate aspect ratio to maintain proportions
    float image_aspect = (float)state.image_width / (float)state.image_height;
    float preview_aspect = width / height;

    float display_width, display_height;
    if (image_aspect > preview_aspect) {
      // Image is wider than preview area
      display_width = width;
      display_height = width / image_aspect;
    } else {
      // Image is taller than preview area
      display_height = height;
      display_width = height * image_aspect;
    }

    // Center the image
    float pos_x = (width - display_width) * 0.5f;
    float pos_y = (height - display_height) * 0.5f;

    ImGui::SetCursorPos(ImVec2(pos_x, pos_y));
    ImGui::Image((void*)(intptr_t)state.image_texture,
                 ImVec2(display_width, display_height));
  } else {
    // Display placeholder text when no image is loaded
    const float pos_x = width / 2 - 50;
    const float pos_y = height / 2 - 10;
    ImGui::SetCursorPos(ImVec2(pos_x, pos_y));
    ImGui::Text("No image loaded");
  }

  ImGui::EndChild();
  ImGui::PopStyleVar();    // ChildRounding
  ImGui::PopStyleColor();  // ChildBg
}

// Render the buttons, dropdown and slider
void RenderControlsArea(AppState& state, float window_width,
                        float window_height) {
  // Calculate button size based on window size
  const float button_size = window_height * 0.08f;

  // Use columns for layout
  ImGui::Columns(3, "ControlsColumns", false);

  // First column: Load/Convert buttons and combo box
  ImGui::BeginGroup();

  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                      ImVec2(button_size * 0.2f, button_size * 0.2f));

  // Load and Convert buttons
  if (ImGui::Button("Load", ImVec2(button_size, button_size))) {
    // Load button clicked
  }

  ImGui::SameLine();

  if (ImGui::Button("Convert", ImVec2(button_size, button_size))) {
    // Convert button clicked
  }

  // Conversion combo box
  const float combo_width =
      ImGui::GetColumnWidth() - ImGui::GetStyle().FramePadding.x * 2;
  ImGui::PushItemWidth(combo_width);

  // Add spacing before the combo box
  ImGui::Dummy(ImVec2(0, button_size * 0.2f));

  // Style the combo box
  ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.3f, 0.3f, 0.7f, 0.8f));
  ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.4f, 0.4f, 0.8f, 0.8f));
  ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.5f, 0.5f, 0.9f, 0.8f));

  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 5.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 8));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));

  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));

  // Label and combo box
  ImGui::Text("Conversion Type:");

  // Put the combo on a new line for better visibility
  if (ImGui::BeginCombo(
          "##ConversionCombo",
          state.conversion_options[state.current_conversion].c_str(),
          ImGuiComboFlags_HeightLarge)) {
    for (int i = 0; i < state.conversion_options.size(); i++) {
      const bool is_selected = (state.current_conversion == i);
      if (ImGui::Selectable(state.conversion_options[i].c_str(), is_selected, 0,
                            ImVec2(0, 30))) {
        state.current_conversion = i;
      }
      if (is_selected) {
        ImGui::SetItemDefaultFocus();
      }
    }
    ImGui::EndCombo();
  }

  // Pop all the styles
  ImGui::PopStyleColor(7);
  ImGui::PopStyleVar(4);
  ImGui::PopItemWidth();
  ImGui::PopStyleVar();  // ItemSpacing

  ImGui::EndGroup();

  ImGui::NextColumn();

  // Second column: Spacer
  ImGui::Dummy(ImVec2(ImGui::GetColumnWidth() * 0.5f, button_size * 0.2f));

  ImGui::NextColumn();

  // Third column: Slider
  ImGui::PushItemWidth(ImGui::GetColumnWidth() -
                       ImGui::GetStyle().FramePadding.x * 2);

  // Style the slider
  ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.4f, 0.4f, 0.8f, 0.8f));
  ImGui::PushStyleColor(ImGuiCol_SliderGrabActive,
                        ImVec4(0.5f, 0.5f, 0.9f, 0.8f));
  ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 10.0f);

  ImGui::SliderInt("##Slider", &state.slider_value, 0, 100, "%d%%");

  ImGui::PopStyleColor(3);
  ImGui::PopStyleVar();
  ImGui::PopItemWidth();

  ImGui::Columns(1);
}

// Main application function
void RenderMainUI(AppState& state) {
  // Set window to be fullscreen with padding
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

  const ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoScrollbar |
      ImGuiWindowFlags_NoScrollWithMouse;

  // Set padding to zero to avoid extra space
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

  ImGui::Begin("PSM GUI", nullptr, window_flags);

  // Remove the previous padding style var
  ImGui::PopStyleVar();

  // Get the current window size for scaling calculations
  const float window_width = ImGui::GetWindowWidth();
  const float window_height = ImGui::GetWindowHeight();

  // Calculate padding based on window size (responsive)
  const float padding_x = window_width * 0.025f;
  const float padding_y = window_height * 0.03f;

  // Add padding for content
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(padding_x, padding_y));

  // Set content region with padding
  ImGui::SetCursorPos(ImVec2(padding_x, padding_y));
  const float content_width = window_width - (padding_x * 2);
  const float content_height = window_height - (padding_y * 2);

  ImGui::BeginChild(
      "Content", ImVec2(content_width, content_height), false,
      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

  // Calculate sizes for preview and controls
  const float controls_height = window_height * 0.15f;
  const float spacing = window_height * 0.03f;
  const float preview_height =
      ImGui::GetContentRegionAvail().y - controls_height - spacing;
  const float preview_width = ImGui::GetContentRegionAvail().x;

  // Render the preview area
  RenderPreviewArea(state, preview_width, preview_height);

  // Vertical spacer
  ImGui::Dummy(ImVec2(0, spacing));

  // Render the controls area
  RenderControlsArea(state, window_width, window_height);

  ImGui::EndChild();  // End Content child

  ImGui::PopStyleVar();  // FramePadding

  ImGui::End();
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

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Initialize application state
  AppState state;

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

    // Render the main UI
    RenderMainUI(state);

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
