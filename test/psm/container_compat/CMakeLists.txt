add_executable(container_compat_test container_compat_test.cpp)
target_link_libraries(container_compat_test PRIVATE psm::psm)
addtests(container_compat_test)
