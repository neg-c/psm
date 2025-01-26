include(GNUInstallDirs)

set(INSTALL_TARGETS psm psm_srgb) # Always include core psm and psm_srgb

foreach(module ${PSM_MODULES})
  string(TOUPPER "${module}" MODULE_UPPER)
  if(WITH_${MODULE_UPPER})
    list(APPEND INSTALL_TARGETS psm_${module})
  endif()
endforeach()

# Install libraries and headers with components
install(
  TARGETS ${INSTALL_TARGETS}
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
  FILES "${CMAKE_SOURCE_DIR}/LICENSE" "${CMAKE_SOURCE_DIR}/README.md"
        "${CMAKE_SOURCE_DIR}/INSTALL.md" "${CMAKE_SOURCE_DIR}/CHANGELOG.md"
        "${CMAKE_SOURCE_DIR}/AUTHORS.md"
  DESTINATION ${CMAKE_INSTALL_DOCDIR}
  COMPONENT documentation)

# Install CMake module files helper
install(
  FILES "${CMAKE_SOURCE_DIR}/cmake/PsmModule.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/psm
  COMPONENT development)
