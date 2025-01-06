add_executable(psm_cli psm_cli.cpp)

find_package(OpenCV REQUIRED)

target_link_libraries(psm_cli PRIVATE psm opencv_core opencv_imgcodecs
                                      opencv_imgproc opencv_highgui)

if(MSVC)
  add_custom_command(
    TARGET psm_cli
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:psm_cli>
            $<TARGET_RUNTIME_DLLS:psm_cli>
    COMMAND_EXPAND_LISTS)
endif()
