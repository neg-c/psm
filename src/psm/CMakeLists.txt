add_subdirectory(orgb)

add_library(psm INTERFACE)
target_link_libraries(psm INTERFACE psm_orgb)
