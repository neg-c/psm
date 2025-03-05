add_executable(adobe_rgb_test adobe_rgb_test.cpp)

target_link_libraries(adobe_rgb_test PRIVATE psm::adobe_rgb psm_test_utils)

addtests(adobe_rgb_test)
