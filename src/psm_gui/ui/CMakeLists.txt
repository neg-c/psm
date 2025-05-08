add_libarary(psm_gui_ui_renderer STATIC)

target_sources(psm_gui_ui_renderer PRIVATE UIRenderer.cpp)
target_include_directories(psm_gui_ui_renderer
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(psm_gui_ui_renderer PRIVATE glfw OpenGL::GL imgui::imgui)
