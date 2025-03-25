#include "ui_renderer.h"

#include "imgui.h"

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
