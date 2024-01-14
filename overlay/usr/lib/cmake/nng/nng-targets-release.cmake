#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "nng::nng" for configuration "Release"
set_property(TARGET nng::nng APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(nng::nng PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libnng.so.1.6.0-pre"
  IMPORTED_SONAME_RELEASE "libnng.so.1"
  )

list(APPEND _cmake_import_check_targets nng::nng )
list(APPEND _cmake_import_check_files_for_nng::nng "${_IMPORT_PREFIX}/lib/libnng.so.1.6.0-pre" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
