add_library(psm_gui_ui STATIC)

target_sources(psm_gui_ui PRIVATE UIRenderer.cpp)
target_include_directories(
  psm_gui_ui
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(psm_gui_ui PRIVATE glfw OpenGL::GL imgui::imgui)
