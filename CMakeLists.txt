add_library(ImageProcessor INTERFACE ImageProcessor.hpp)

target_include_directories(
  ImageProcessor INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                           "$<INSTALL_INTERFACE:${CMAKE_INSTALL_LIBDIR}>")
target_compile_features(ImageProcessor INTERFACE cxx_std_20)
