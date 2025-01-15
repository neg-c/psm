add_subdirectory(detail)
add_subdirectory(srgb)
add_subdirectory(orgb)
add_subdirectory(adobe_rgb)


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
         include/psm/color_space_concept.hpp
         include/psm/percent.hpp
         ${CMAKE_BINARY_DIR}/include/psm/version.hpp)

target_link_libraries(psm INTERFACE psm_detail psm_orgb psm_srgb psm_adobe_rgb)
