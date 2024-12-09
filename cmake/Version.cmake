include(CMakePackageConfigHelpers)

if(DEFINED PROJECT_VERSION_SUFFIX)
  set(FULL_PROJECT_VERSION "${PROJECT_VERSION}-${PROJECT_VERSION_SUFFIX}")
else()
  set(FULL_PROJECT_VERSION "${PROJECT_VERSION}")
endif()

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/psmConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.hpp.in"
               "${CMAKE_CURRENT_BINARY_DIR}/include/psm/version.hpp" @ONLY)
