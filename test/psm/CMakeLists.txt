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
