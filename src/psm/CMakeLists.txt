add_library(${CMAKE_PROJECT_NAME} SHARED)
add_library(${CMAKE_PROJECT_NAME}_orgb SHARED)

target_sources(
  ${CMAKE_PROJECT_NAME}
  PUBLIC FILE_SET
         modules
         TYPE
         CXX_MODULES
         BASE_DIRS
         "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
         "$<INSTALL_INTERFACE:${CMAKE_INSTALL_LIBDIR}/${CMAKE_PROJECT_NAME}>"
         FILES
         ${CMAKE_PROJECT_NAME}.ixx)

target_sources(
  ${CMAKE_PROJECT_NAME}_orgb
  PRIVATE orgb.cpp
  PUBLIC FILE_SET
         modules
         TYPE
         CXX_MODULES
         BASE_DIRS
         "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
         "$<INSTALL_INTERFACE:${CMAKE_INSTALL_LIBDIR}/${CMAKE_PROJECT_NAME}>"
         FILES
         orgb.ixx)

find_package(Eigen3 REQUIRED)

target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_PROJECT_NAME}_orgb)
target_link_libraries(${CMAKE_PROJECT_NAME}_orgb PRIVATE Eigen3::Eigen)
target_compile_features(${CMAKE_PROJECT_NAME} PUBLIC cxx_std_20)
target_compile_features(${CMAKE_PROJECT_NAME}_orgb PUBLIC cxx_std_20)
