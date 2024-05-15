qt_add_library(ImageProcessor STATIC ImageProcessor.cpp)

find_package(Eigen3 REQUIRED)

target_link_libraries(ImageProcessor PRIVATE Qt6::Core Qt6::Gui Eigen3::Eigen)
target_include_directories(ImageProcessor INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
