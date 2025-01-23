add_library(psm_orgb SHARED)

target_sources(
  psm_orgb
  PRIVATE orgb.cpp
  PUBLIC FILE_SET HEADERS BASE_DIRS ${CMAKE_SOURCE_DIR}/src/psm/include FILES
         ${CMAKE_SOURCE_DIR}/src/psm/include/psm/detail/orgb.hpp)

find_package(Eigen3 REQUIRED)

target_link_libraries(psm_orgb PRIVATE Eigen3::Eigen psm_srgb)
target_compile_features(psm_orgb PUBLIC cxx_std_20)
