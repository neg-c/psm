include(GNUInstallDirs)

# Install libraries and headers with components
install(
  TARGETS psm psm_orgb
  EXPORT psmLibrary
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          FILE_SET HEADERS
          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
          COMPONENT applications)

# Install CLI tool
install(TARGETS psm_cli RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
                                COMPONENT applications)

# Run ldconfig on Unix systems
if(UNIX)
  install(CODE "execute_process(COMMAND ldconfig)" COMPONENT applications)
endif()

# Install CMake configuration files
install(
  EXPORT psmLibrary
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/psm
  NAMESPACE psm::
  COMPONENT headers)

# Install CMake package configuration files
install(
  FILES "${CMAKE_SOURCE_DIR}/cmake/psmConfig.cmake"
        "${CMAKE_BINARY_DIR}/cmake/psmConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/psm
  COMPONENT applications)

# Install documentation files
install(
  FILES 
    "${CMAKE_SOURCE_DIR}/LICENSE"
    "${CMAKE_SOURCE_DIR}/README.md"
    "${CMAKE_SOURCE_DIR}/INSTALL.md"
    "${CMAKE_SOURCE_DIR}/CHANGELOG.md"
    "${CMAKE_SOURCE_DIR}/AUTHORS.md"
  DESTINATION ${CMAKE_INSTALL_DOCDIR}
  COMPONENT documentation)
