add_library(psm_gui_ui STATIC)

target_sources(
  psm_gui_ui
  PRIVATE UIRenderer.cpp
          panels/Toolbar.cpp
          panels/ControlPanel.cpp
          panels/PreviewArea.cpp
          panels/PixelInfo.cpp
          panels/HorizontalSlider.cpp
          panels/VerticalSlider.cpp)
target_include_directories(
  psm_gui_ui
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(psm_gui_ui PRIVATE glfw OpenGL::GL imgui::imgui
                                         psm_gui_controller)
target_compile_features(psm_gui_ui PUBLIC cxx_std_20)
