add_library(${CMAKE_PROJECT_NAME} SHARED)
add_library(${CMAKE_PROJECT_NAME}_orgb SHARED)

target_sources(
  ${CMAKE_PROJECT_NAME}
  PRIVATE psm.cpp
  PUBLIC FILE_SET HEADERS BASE_DIRS include FILES include/psm/psm.hpp)

target_sources(
  ${CMAKE_PROJECT_NAME}_orgb
  PRIVATE orgb.cpp
  PUBLIC FILE_SET HEADERS BASE_DIRS include FILES include/psm/orgb.hpp)

find_package(Eigen3 REQUIRED)

target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_PROJECT_NAME}_orgb)
target_link_libraries(${CMAKE_PROJECT_NAME}_orgb PRIVATE Eigen3::Eigen)
target_compile_features(${CMAKE_PROJECT_NAME} PUBLIC cxx_std_20)
target_compile_features(${CMAKE_PROJECT_NAME}_orgb PUBLIC cxx_std_20)
