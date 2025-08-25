add_library(psm_image_processor STATIC)

target_sources(psm_image_processor PRIVATE image_processor.cpp)

target_include_directories(
  psm_image_processor
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..
  PUBLIC ${CMAKE_SOURCE_DIR}/src/psm/include
  PUBLIC ${CMAKE_SOURCE_DIR}/src/app/cli)

target_link_libraries(psm_image_processor PUBLIC psm::psm psm::adjust_channels
                                                 psm::image_io)

target_compile_features(psm_image_processor PUBLIC cxx_std_20)
