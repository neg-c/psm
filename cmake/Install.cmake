include(GNUInstallDirs)
install(
  TARGETS psm psm_orgb
  EXPORT psmLibrary
  FILE_SET HEADERS)

if(UNIX)
  install(CODE "execute_process(COMMAND ldconfig)")
endif()

install(
  EXPORT psmLibrary
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/psm/cmake
  NAMESPACE psm::)

install(FILES "${CMAKE_SOURCE_DIR}/cmake/psmConfig.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/psm/cmake)

install(TARGETS psm_cli RUNTIME)
