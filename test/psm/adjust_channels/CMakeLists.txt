add_executable(adjust_channels_test adjust_channels_test.cpp)

target_link_libraries(adjust_channels_test
  PRIVATE
    psm::adjust_channels
)
AddTests(adjust_channels_test)
