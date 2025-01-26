add_library(psm_srgb SHARED)
# This module is not going be exposed to the client so an add_library will
# suffice

target_sources(
  psm_srgb
  PRIVATE srgb.cpp
  PUBLIC FILE_SET HEADERS BASE_DIRS ${CMAKE_SOURCE_DIR}/src/psm/include FILES
         ${CMAKE_SOURCE_DIR}/src/psm/include/psm/detail/srgb.hpp)

target_compile_features(psm_srgb PUBLIC cxx_std_20)
