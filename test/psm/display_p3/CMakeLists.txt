add_executable(display_p3_test display_p3_test.cpp)
target_link_libraries(display_p3_test PRIVATE psm::display_p3 psm_test_utils)
addtests(display_p3_test)
