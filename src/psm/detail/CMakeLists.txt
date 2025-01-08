add_library(psm_detail
  adjust_channels.cpp
)

find_package(Eigen3 REQUIRED)

target_include_directories(psm_detail PUBLIC ${CMAKE_SOURCE_DIR}/src/psm/include)
target_link_libraries(psm_detail PRIVATE Eigen3::Eigen)

target_compile_features(psm_detail PUBLIC cxx_std_20)