set(PSM_MODULES adjust_channels orgb adobe_rgb)

# Dynamically create WITH_<module> options
foreach(module ${PSM_MODULES})
  string(TOUPPER "${module}" MODULE_UPPER) # Convert to uppercase for
                                           # WITH_<module>
  option(WITH_${MODULE_UPPER} "Include the ${module} module" OFF)
endforeach()

function(all_off result)
  set(all_off_flag ON)
  foreach(module ${PSM_MODULES})
    string(TOUPPER "${module}" MODULE_UPPER)
    if(WITH_${MODULE_UPPER})
      set(all_off_flag OFF)
      break()
    endif()
  endforeach()
  set(${result}
      ${all_off_flag}
      PARENT_SCOPE)
endfunction()

# Determine whether all options are OFF
all_off(ALL_OFF)

# If all options are OFF, build everything by default
if(ALL_OFF)
  foreach(module ${PSM_MODULES})
    string(TOUPPER "${module}" MODULE_UPPER)
    set(WITH_${MODULE_UPPER}
        ON
        CACHE BOOL "Include the ${module} module" FORCE)
  endforeach()
endif()
