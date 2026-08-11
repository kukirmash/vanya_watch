#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "lvgl::lvgl_demos" for configuration "Debug"
set_property(TARGET lvgl::lvgl_demos APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(lvgl::lvgl_demos PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib64/liblvgl_demos.a"
  )

list(APPEND _cmake_import_check_targets lvgl::lvgl_demos )
list(APPEND _cmake_import_check_files_for_lvgl::lvgl_demos "${_IMPORT_PREFIX}/lib64/liblvgl_demos.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
