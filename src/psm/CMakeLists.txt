add_subdirectory(orgb)
add_subdirectory(srgb)

add_library(psm INTERFACE)

target_sources(
  psm
  PUBLIC FILE_SET
         HEADERS
         BASE_DIRS
         include
         ${CMAKE_BINARY_DIR}/include/
         FILES
         include/psm/psm.hpp
         include/psm/color_space.hpp
         ${CMAKE_BINARY_DIR}/include/psm/version.hpp)

target_link_libraries(psm INTERFACE psm_orgb psm_srgb)
