add_executable(orgb_test orgb_test.cpp)
target_link_libraries(orgb_test PRIVATE psm::orgb)
addtests(orgb_test)
