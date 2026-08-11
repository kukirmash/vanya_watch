# Install script for directory: /home/kukirmash/vanya_watch/linux_project/lvgl

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES "/home/kukirmash/vanya_watch/linux_project/lvgl/include/lvgl")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/lvgl/config" TYPE FILE RENAME "lv_conf.h" FILES "/home/kukirmash/vanya_watch/linux_project/lv_conf.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64/pkgconfig" TYPE FILE FILES "/home/kukirmash/vanya_watch/linux_project/build/lvgl/lvgl.pc")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64/cmake/lvgl" TYPE FILE FILES
    "/home/kukirmash/vanya_watch/linux_project/build/lvgl/lvglConfig.cmake"
    "/home/kukirmash/vanya_watch/linux_project/build/lvgl/lvglConfigVersion.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64" TYPE STATIC_LIBRARY FILES "/home/kukirmash/vanya_watch/linux_project/build/lvgl/liblvgl.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/cmake/lvgl/lvglTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/cmake/lvgl/lvglTargets.cmake"
         "/home/kukirmash/vanya_watch/linux_project/build/lvgl/CMakeFiles/Export/14fa9370ebbbb32762b2db5c20cda8b3/lvglTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/cmake/lvgl/lvglTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/cmake/lvgl/lvglTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64/cmake/lvgl" TYPE FILE FILES "/home/kukirmash/vanya_watch/linux_project/build/lvgl/CMakeFiles/Export/14fa9370ebbbb32762b2db5c20cda8b3/lvglTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64/cmake/lvgl" TYPE FILE FILES "/home/kukirmash/vanya_watch/linux_project/build/lvgl/CMakeFiles/Export/14fa9370ebbbb32762b2db5c20cda8b3/lvglTargets-debug.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64" TYPE STATIC_LIBRARY FILES "/home/kukirmash/vanya_watch/linux_project/build/lvgl/lib/liblvgl_thorvg.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/lvgl" TYPE DIRECTORY FILES "/home/kukirmash/vanya_watch/linux_project/lvgl/demos" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64" TYPE STATIC_LIBRARY FILES "/home/kukirmash/vanya_watch/linux_project/build/lvgl/liblvgl_demos.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/cmake/lvgl/lvglDemosTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/cmake/lvgl/lvglDemosTargets.cmake"
         "/home/kukirmash/vanya_watch/linux_project/build/lvgl/CMakeFiles/Export/14fa9370ebbbb32762b2db5c20cda8b3/lvglDemosTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/cmake/lvgl/lvglDemosTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/cmake/lvgl/lvglDemosTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64/cmake/lvgl" TYPE FILE FILES "/home/kukirmash/vanya_watch/linux_project/build/lvgl/CMakeFiles/Export/14fa9370ebbbb32762b2db5c20cda8b3/lvglDemosTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64/cmake/lvgl" TYPE FILE FILES "/home/kukirmash/vanya_watch/linux_project/build/lvgl/CMakeFiles/Export/14fa9370ebbbb32762b2db5c20cda8b3/lvglDemosTargets-debug.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/lvgl" TYPE DIRECTORY FILES "/home/kukirmash/vanya_watch/linux_project/lvgl/examples" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64" TYPE STATIC_LIBRARY FILES "/home/kukirmash/vanya_watch/linux_project/build/lvgl/liblvgl_examples.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/cmake/lvgl/lvglExamplesTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/cmake/lvgl/lvglExamplesTargets.cmake"
         "/home/kukirmash/vanya_watch/linux_project/build/lvgl/CMakeFiles/Export/14fa9370ebbbb32762b2db5c20cda8b3/lvglExamplesTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/cmake/lvgl/lvglExamplesTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/cmake/lvgl/lvglExamplesTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64/cmake/lvgl" TYPE FILE FILES "/home/kukirmash/vanya_watch/linux_project/build/lvgl/CMakeFiles/Export/14fa9370ebbbb32762b2db5c20cda8b3/lvglExamplesTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64/cmake/lvgl" TYPE FILE FILES "/home/kukirmash/vanya_watch/linux_project/build/lvgl/CMakeFiles/Export/14fa9370ebbbb32762b2db5c20cda8b3/lvglExamplesTargets-debug.cmake")
  endif()
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/kukirmash/vanya_watch/linux_project/build/lvgl/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
