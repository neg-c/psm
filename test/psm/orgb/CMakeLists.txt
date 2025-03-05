add_executable(orgb_test orgb_test.cpp)
target_link_libraries(orgb_test PRIVATE psm::orgb psm_test_utils)
addtests(orgb_test)
