include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/psmConfigVersion.cmake
  COMPATIBILITY AnyNewerVersion)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.hpp.in"
               "${CMAKE_CURRENT_BINARY_DIR}/include/version.hpp" @ONLY)
