add_executable(${CMAKE_PROJECT_NAME}_cli ${CMAKE_PROJECT_NAME}_cli.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME}_cli PRIVATE ${CMAKE_PROJECT_NAME})
