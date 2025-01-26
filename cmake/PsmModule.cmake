function(psm_add_module name type)
  string(SUBSTRING "${name}" 0 4 prefix)
  if(NOT prefix STREQUAL "psm_")
    message(FATAL_ERROR "Module name '${name}' must start with 'psm_' prefix")
  endif()
  string(SUBSTRING "${name}" 4 -1 alias_name)
  add_library(${name} ${type})
  add_library(psm::${alias_name} ALIAS ${name})
endfunction()

#[=======================================================================[.rst:
psm_add_module
-------------

Creates a new PSM module with proper namespace aliasing and export configuration.

.. command:: psm_add_module(name type)

  Creates a new PSM module library with the specified name and type.

  ``name``
    The name of the module. Must start with ``psm_`` prefix.
    Example: psm_adobe_rgb

  ``type``
    The library type, typically SHARED or STATIC.
    Example: SHARED

Example usage:

.. code-block:: cmake

  psm_add_module(psm_my_module SHARED)

  target_sources(psm_my_module
    PRIVATE
      src/my_module.cpp
  )

  target_link_libraries(psm_my_module
    PRIVATE
      psm::core
  )

Notes:
  * The module will be automatically available under the psm:: namespace
    (e.g., psm::my_module)
  * All modules must follow the psm_ prefix convention
  * This is the preferred way to add new optional modules to PSM
#]=======================================================================]
