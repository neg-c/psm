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

add_subdirectory(srgb)
target_link_libraries(psm INTERFACE psm_srgb)

if(PSM_BUILD_ADJUST_CHANNELS)
  add_subdirectory(adjust_channels)
  target_link_libraries(psm INTERFACE psm_adjust_channels)
endif()

if(PSM_BUILD_ORGB)
  add_subdirectory(orgb)
  target_link_libraries(psm INTERFACE psm_orgb)
endif()

if(PSM_BUILD_ADOBE_RGB)
  add_subdirectory(adobe_rgb)
  target_link_libraries(psm INTERFACE psm_adobe_rgb)
endif()
