add_executable(pro_photo_rgb_test pro_photo_rgb_test.cpp)

target_link_libraries(pro_photo_rgb_test PRIVATE psm::pro_photo_rgb
                                                 psm_test_utils)

addtests(pro_photo_rgb_test)
