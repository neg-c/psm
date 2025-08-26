add_executable(percent_test percent_test.cpp)
target_link_libraries(percent_test PRIVATE psm::psm)
addtests(percent_test)
