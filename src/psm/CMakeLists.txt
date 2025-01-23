add_library(psm INTERFACE)
add_library(psm::psm ALIAS psm)

target_sources(
  psm
  PUBLIC FILE_SET
         HEADERS
         BASE_DIRS
         include
         ${CMAKE_BINARY_DIR}/include/
         FILES
         include/psm/psm.hpp
         include/psm/detail/color_space_concept.hpp
         ${CMAKE_BINARY_DIR}/include/psm/version.hpp)

# Always include the default srgb module
add_subdirectory(srgb)
target_link_libraries(psm INTERFACE psm_srgb)

# Conditionally add other modules based on WITH_<module> flags
foreach(module ${PSM_MODULES})
  string(TOUPPER "${module}" MODULE_UPPER)
  if(WITH_${MODULE_UPPER})
    add_subdirectory(${module})
    target_link_libraries(psm INTERFACE psm_${module})
    set_target_properties(psm_${module} PROPERTIES EXPORT_NAME ${module})
  endif()
endforeach()
