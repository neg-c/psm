psm_add_module(psm_adjust_channels SHARED)

target_sources(
  psm_adjust_channels
  PRIVATE adjust_channels.cpp
  PUBLIC FILE_SET
         HEADERS
         BASE_DIRS
         ${CMAKE_SOURCE_DIR}/src/psm/include
         FILES
         ${CMAKE_SOURCE_DIR}/src/psm/include/psm/adjust_channels.hpp
         ${CMAKE_SOURCE_DIR}/src/psm/include/psm/percent.hpp)

find_package(Eigen3 REQUIRED)

target_link_libraries(psm_adjust_channels PRIVATE Eigen3::Eigen)
target_compile_features(psm_adjust_channels PUBLIC cxx_std_20)
