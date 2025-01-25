enable_testing()

find_package(GTest CONFIG REQUIRED)

include(GoogleTest)
# TODO: include Coverage module

macro(AddTests target)
  target_link_libraries(${target} PRIVATE GTest::gtest GTest::gtest_main
                                          GTest::gmock)
  gtest_discover_tests(${target})
  # TODO: Add coverage
endmacro()
