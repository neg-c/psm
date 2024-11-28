add_library(psm INTERFACE)
add_library(psm_orgb SHARED)

target_sources(
  psm
  PUBLIC FILE_SET
         HEADERS
         BASE_DIRS
         include
         ${CMAKE_BINARY_DIR}/include/
         FILES
         include/psm/psm.hpp
         ${CMAKE_BINARY_DIR}/include/psm/version.hpp)

target_sources(
  psm_orgb
  PRIVATE orgb.cpp
  PUBLIC FILE_SET HEADERS BASE_DIRS include FILES include/psm/orgb.hpp)

find_package(Eigen3 REQUIRED)

target_link_libraries(psm INTERFACE psm_orgb)
target_link_libraries(psm_orgb PRIVATE Eigen3::Eigen)
target_compile_features(psm INTERFACE cxx_std_20)
target_compile_features(psm_orgb PUBLIC cxx_std_20)
