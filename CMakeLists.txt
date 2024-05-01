qt_add_library(ImageProcessor STATIC ImageProcessor.cpp)

target_link_libraries(ImageProcessor PRIVATE Qt6::Core Qt6::Gui)
target_include_directories(ImageProcessor INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
