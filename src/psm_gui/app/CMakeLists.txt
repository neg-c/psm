add_executable(psm_gui main.cpp Application.cpp)

target_include_directories(psm_gui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(psm_gui PRIVATE glfw OpenGL::GL imgui::imgui)

if(MSVC)
  add_custom_command(
    TARGET psm_gui
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:psm_gui>
    COMMAND_EXPAND_LISTS)
endif()
