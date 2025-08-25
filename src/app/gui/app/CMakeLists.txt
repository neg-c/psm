add_executable(psm_gui main.cpp Application.cpp)

find_package(nfd REQUIRED)

target_include_directories(psm_gui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
                                           ${CMAKE_SOURCE_DIR}/src/psm/include)

target_link_libraries(psm_gui PRIVATE glfw OpenGL::GL imgui::imgui psm_gui_ui
                                      nfd::nfd)

target_compile_features(psm_gui PUBLIC cxx_std_20)

if(MSVC)
  target_link_options(psm_gui PRIVATE "/SUBSYSTEM:WINDOWS"
                      "/ENTRY:mainCRTStartup")
  add_custom_command(
    TARGET psm_gui
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:psm_gui>
            $<TARGET_RUNTIME_DLLS:psm_gui>
    COMMAND_EXPAND_LISTS)
endif()
