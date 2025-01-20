add_library(psm_adjust_channels SHARED adjust_channels.cpp)

find_package(Eigen3 REQUIRED)

target_include_directories(psm_adjust_channels
                           PRIVATE ${CMAKE_SOURCE_DIR}/src/psm/include)

target_link_libraries(psm_adjust_channels PRIVATE Eigen3::Eigen)

target_compile_features(psm_adjust_channels PUBLIC cxx_std_20)
