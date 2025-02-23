psm_add_module(psm_display_p3 SHARED)

target_sources(
  psm_display_p3
  PRIVATE display_p3.cpp
  PUBLIC FILE_SET HEADERS BASE_DIRS ${CMAKE_SOURCE_DIR}/src/psm/include FILES
         ${CMAKE_SOURCE_DIR}/src/psm/include/psm/detail/display_p3.hpp)

find_package(Eigen3 REQUIRED)

target_link_libraries(psm_display_p3 PRIVATE Eigen3::Eigen psm_srgb)
target_compile_features(psm_display_p3 PUBLIC cxx_std_20)
