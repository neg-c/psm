add_library(psm_gui_controller STATIC)

target_sources(psm_gui_controller PRIVATE ToolbarController.cpp
                                          PreviewController.cpp)

target_include_directories(
  psm_gui_controller
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(psm_gui_controller PRIVATE glfw OpenGL::GL GLEW::GLEW imgui::imgui
                                                 psm::psm)
target_compile_features(psm_gui_controller PUBLIC cxx_std_20)
