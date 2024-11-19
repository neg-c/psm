add_library(psm SHARED)
add_library(psm_orgb SHARED)

target_sources(
  psm
  PRIVATE psm.cpp
  PUBLIC FILE_SET HEADERS BASE_DIRS include FILES include/psm/psm.hpp)

target_sources(
  psm_orgb
  PRIVATE orgb.cpp
  PUBLIC FILE_SET HEADERS BASE_DIRS include FILES include/psm/orgb.hpp)

find_package(Eigen3 REQUIRED)

target_link_libraries(psm PUBLIC psm_orgb)
target_link_libraries(psm_orgb PRIVATE Eigen3::Eigen)
target_compile_features(psm PUBLIC cxx_std_20)
target_compile_features(psm_orgb PUBLIC cxx_std_20)
