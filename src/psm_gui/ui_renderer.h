#pragma once

#include "app_state.h"

// Render the preview area with the image or placeholder
void RenderPreviewArea(AppState& state, float width, float height);

// Render the buttons, dropdown and slider
void RenderControlsArea(AppState& state, float window_width,
                        float window_height);

// Main application function
void RenderMainUI(AppState& state);
