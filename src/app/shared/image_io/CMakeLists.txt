# Shared Image I/O Library
add_library(psm_image_io image_io.cpp image_io.hpp)

# Set C++20 standard
target_compile_features(psm_image_io PUBLIC cxx_std_20)

# Find required packages
find_package(PNG REQUIRED)
find_package(libjpeg-turbo CONFIG REQUIRED)

# Link dependencies
target_link_libraries(
  psm_image_io
  PUBLIC
    PNG::PNG
    $<IF:$<TARGET_EXISTS:libjpeg-turbo::turbojpeg>,libjpeg-turbo::turbojpeg,libjpeg-turbo::turbojpeg-static>
)

# Set include directories
target_include_directories(psm_image_io PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Export target
add_library(psm::image_io ALIAS psm_image_io)
