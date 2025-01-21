add_subdirectory(adjust_channels)
add_subdirectory(srgb)
add_subdirectory(orgb)
add_subdirectory(adobe_rgb)

add_library(psm INTERFACE)
add_library(psm::psm ALIAS psm)

target_sources(
  psm
  PUBLIC FILE_SET
         HEADERS
         BASE_DIRS
         include
         ${CMAKE_BINARY_DIR}/include/
         FILES
         include/psm/psm.hpp
         include/psm/detail/color_space_concept.hpp
         ${CMAKE_BINARY_DIR}/include/psm/version.hpp)

target_link_libraries(psm INTERFACE psm_orgb psm_srgb psm_adobe_rgb)
