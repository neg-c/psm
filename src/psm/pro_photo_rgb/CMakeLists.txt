psm_add_module(psm_pro_photo_rgb SHARED)

target_sources(
  psm_pro_photo_rgb
  PRIVATE pro_photo_rgb.cpp
  PUBLIC FILE_SET HEADERS BASE_DIRS ${CMAKE_SOURCE_DIR}/src/psm/include FILES
         ${CMAKE_SOURCE_DIR}/src/psm/include/psm/detail/pro_photo_rgb.hpp)

find_package(Eigen3 REQUIRED)

target_link_libraries(psm_pro_photo_rgb PRIVATE Eigen3::Eigen psm_srgb)
target_compile_features(psm_pro_photo_rgb PUBLIC cxx_std_20)
