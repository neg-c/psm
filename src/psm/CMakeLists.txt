add_library(psm INTERFACE)

target_sources(
  psm
  PUBLIC FILE_SET
         HEADERS
         BASE_DIRS
         include
         ${CMAKE_BINARY_DIR}/include/
         FILES
         include/psm/psm.hpp
         ${CMAKE_BINARY_DIR}/include/psm/version.hpp)

add_subdirectory(orgb)

target_link_libraries(psm INTERFACE psm_orgb)
target_compile_features(psm INTERFACE cxx_std_20)
