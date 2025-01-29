enable_testing()

find_package(GTest CONFIG REQUIRED)

include(GoogleTest)
# TODO: include Coverage module

macro(AddTests target)
  target_link_libraries(${target} PRIVATE GTest::gtest GTest::gtest_main
                                          GTest::gmock)
  if(MSVC)
    add_custom_command(
      TARGET ${target}
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:${target}>
              $<TARGET_RUNTIME_DLLS:${target}>
      COMMAND_EXPAND_LISTS)
  endif()

  gtest_discover_tests(${target})
  # TODO: Add coverage
endmacro()
