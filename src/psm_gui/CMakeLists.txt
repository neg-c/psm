find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(imgui REQUIRED)
find_package(GLEW REQUIRED)

add_subdirectory(app)
add_subdirectory(controller)
add_subdirectory(ui)
