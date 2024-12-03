include(GNUInstallDirs)

# Install libraries and headers with components
install(
  TARGETS psm psm_orgb
  EXPORT psmLibrary
  RUNTIME COMPONENT libraries
  LIBRARY COMPONENT libraries
  ARCHIVE COMPONENT libraries
          FILE_SET HEADERS
          COMPONENT headers)

# Install CLI tool
install(TARGETS psm_cli RUNTIME COMPONENT libraries)

# Run ldconfig on Unix systems
if(UNIX)
  install(CODE "execute_process(COMMAND ldconfig)" COMPONENT libraries)
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
  COMPONENT headers)

# Install documentation files
install(
  FILES "${CMAKE_SOURCE_DIR}/LICENSE" "${CMAKE_SOURCE_DIR}/README.md"
  DESTINATION ${CMAKE_INSTALL_DOCDIR}
  COMPONENT documentation)
