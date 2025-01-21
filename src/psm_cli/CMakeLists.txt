add_executable(psm_cli psm_cli.cpp)

target_link_libraries(psm_cli PRIVATE psm::psm psm::adjust_channels)

if(MSVC)
  add_custom_command(
    TARGET psm_cli
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:psm_cli>
            $<TARGET_RUNTIME_DLLS:psm_cli>
    COMMAND_EXPAND_LISTS)
endif()
