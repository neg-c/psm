add_library(psm_test_utils INTERFACE)
target_include_directories(psm_test_utils INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(container_compat)

if(TARGET psm_adjust_channels)
  add_subdirectory(adjust_channels)
endif()

if(TARGET psm_percent)
  add_subdirectory(percent)
endif()

if(TARGET psm_adobe_rgb)
  add_subdirectory(adobe_rgb)
endif()

if(TARGET psm_display_p3)
  add_subdirectory(display_p3)
endif()

if(TARGET psm_orgb)
  add_subdirectory(orgb)
endif()
