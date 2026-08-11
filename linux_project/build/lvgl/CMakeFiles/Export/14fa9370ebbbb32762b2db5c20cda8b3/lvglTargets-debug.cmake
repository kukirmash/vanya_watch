#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "lvgl::lvgl" for configuration "Debug"
set_property(TARGET lvgl::lvgl APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(lvgl::lvgl PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "ASM;C;CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib64/liblvgl.a"
  )

list(APPEND _cmake_import_check_targets lvgl::lvgl )
list(APPEND _cmake_import_check_files_for_lvgl::lvgl "${_IMPORT_PREFIX}/lib64/liblvgl.a" )

# Import target "lvgl::lvgl_thorvg" for configuration "Debug"
set_property(TARGET lvgl::lvgl_thorvg APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(lvgl::lvgl_thorvg PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib64/liblvgl_thorvg.a"
  )

list(APPEND _cmake_import_check_targets lvgl::lvgl_thorvg )
list(APPEND _cmake_import_check_files_for_lvgl::lvgl_thorvg "${_IMPORT_PREFIX}/lib64/liblvgl_thorvg.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
