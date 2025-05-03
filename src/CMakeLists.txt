add_subdirectory(psm)

if(BUILD_PSM_CLI)
  add_subdirectory(psm_cli)
endif()

if(BUILD_PSM_GUI)
  add_subdirectory(psm_gui)
endif()
