add_library(psm SHARED psm.cpp orgb.cpp)

target_sources(
  psm
  PUBLIC FILE_SET
         HEADERS
         BASE_DIRS
         include
         FILES
         include/psm/psm.hpp
         include/psm/orgb.hpp)

target_include_directories(
  psm INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                "$<INSTALL_INTERFACE:${CMAKE_INSTALL_LIBDIR}>")

find_package(Eigen3 REQUIRED)

target_link_libraries(psm PUBLIC Eigen3::Eigen)
target_compile_features(psm PUBLIC cxx_std_20)
