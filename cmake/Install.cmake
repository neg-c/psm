include(GNUInstallDirs)

# Install libraries and headers with components
install(
  TARGETS psm psm_orgb
  EXPORT psmLibrary
  RUNTIME COMPONENT applications
  LIBRARY COMPONENT applications
  ARCHIVE COMPONENT applications
          FILE_SET HEADERS
          COMPONENT applications)

# Install CLI tool
install(TARGETS psm_cli RUNTIME COMPONENT applications)

# Run ldconfig on Unix systems
if(UNIX)
  install(CODE "execute_process(COMMAND ldconfig)" COMPONENT applications)
endif()

# Install CMake configuration files
install(
  EXPORT psmLibrary
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/psm/cmake
  NAMESPACE psm::
  COMPONENT headers)

# Install CMake package configuration files
install(
  FILES "${CMAKE_SOURCE_DIR}/cmake/psmConfig.cmake"
        "${CMAKE_BINARY_DIR}/cmake/psmConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/psm/cmake
  COMPONENT applications)

# Install documentation files
install(
  FILES "${CMAKE_SOURCE_DIR}/LICENSE" "${CMAKE_SOURCE_DIR}/README.md"
  DESTINATION ${CMAKE_INSTALL_DOCDIR}
  COMPONENT documentation)
