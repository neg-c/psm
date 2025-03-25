add_executable(psm_gui main.cpp image_loader.cpp ui_renderer.cpp)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(imgui REQUIRED)
find_package(Stb REQUIRED)

target_include_directories(psm_gui PRIVATE ${Stb_INCLUDE_DIR})

target_link_libraries(psm_gui PRIVATE imgui::imgui glfw OpenGL::GL)

# Define STB_IMAGE_IMPLEMENTATION in one source file
target_compile_definitions(psm_gui PRIVATE STB_IMAGE_IMPLEMENTATION)

if(MSVC)
  add_custom_command(
    TARGET psm_gui
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:psm_gui>
            $<TARGET_RUNTIME_DLLS:psm_gui>
    COMMAND_EXPAND_LISTS)
endif()
