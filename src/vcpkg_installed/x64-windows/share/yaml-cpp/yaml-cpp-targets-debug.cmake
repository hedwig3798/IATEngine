#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "yaml-cpp" for configuration "Debug"
set_property(TARGET yaml-cpp APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(yaml-cpp PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/debug/lib/yaml-cppd.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/bin/yaml-cppd.dll"
  )

list(APPEND _cmake_import_check_targets yaml-cpp )
list(APPEND _cmake_import_check_files_for_yaml-cpp "${_IMPORT_PREFIX}/debug/lib/yaml-cppd.lib" "${_IMPORT_PREFIX}/debug/bin/yaml-cppd.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)