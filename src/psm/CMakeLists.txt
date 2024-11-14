add_library(psm SHARED)
add_library(psm_orgb SHARED)

target_sources(
  psm
  PUBLIC FILE_SET
         modules
         TYPE
         CXX_MODULES
         BASE_DIRS
         "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
         "$<INSTALL_INTERFACE:${CMAKE_INSTALL_LIBDIR}>"
         FILES
         psm.ixx)

target_sources(
  psm_orgb
  PRIVATE orgb.cpp
  PUBLIC FILE_SET
         modules
         TYPE
         CXX_MODULES
         BASE_DIRS
         "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
         "$<INSTALL_INTERFACE:${CMAKE_INSTALL_LIBDIR}>"
         FILES
         orgb.ixx)

find_package(Eigen3 REQUIRED)

target_link_libraries(psm PRIVATE psm_orgb)
target_link_libraries(psm_orgb PUBLIC Eigen3::Eigen)
target_compile_features(psm PUBLIC cxx_std_20)
target_compile_features(psm_orgb PUBLIC cxx_std_20)
