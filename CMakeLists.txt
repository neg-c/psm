add_library(ImageProcessor STATIC ImageProcessor.hpp)

find_package(Eigen3 REQUIRED)

target_include_directories(
  ImageProcessor INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                           "$<INSTALL_INTERFACE:${CMAKE_INSTALL_LIBDIR}>")

target_link_libraries(ImageProcessor PUBLIC Eigen3::Eigen)
target_compile_features(ImageProcessor INTERFACE cxx_std_20)
