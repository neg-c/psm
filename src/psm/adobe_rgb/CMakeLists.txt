add_library(psm_adobe_rgb SHARED)

target_sources(
  psm_adobe_rgb
  PRIVATE adobe_rgb.cpp
  PUBLIC FILE_SET HEADERS BASE_DIRS ${CMAKE_SOURCE_DIR}/src/psm/include FILES
         ${CMAKE_SOURCE_DIR}/src/psm/include/psm/detail/adobe_rgb.hpp)

find_package(Eigen3 REQUIRED)

target_link_libraries(psm_adobe_rgb PRIVATE Eigen3::Eigen psm_srgb)
target_compile_features(psm_adobe_rgb PUBLIC cxx_std_20)
