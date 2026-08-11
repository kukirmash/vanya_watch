cmake_minimum_required(VERSION 3.11)
project(vanya_watch_sim)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# 1. ОТКЛЮЧАЕМ KCONFIG
set(LV_BUILD_USE_KCONFIG OFF CACHE BOOL "Disable Kconfig" FORCE)
set(LV_CONF_BUILD_DISABLE_AUTO_INCLUDE ON CACHE BOOL "Disable auto include" FORCE)

# ДОБАВЛЕНО: Глобально указываем, где лежит lv_conf.h и включаем макрос
include_directories(${CMAKE_CURRENT_LIST_DIR})
add_compile_definitions(LV_CONF_INCLUDE_SIMPLE=1)

# 2. Подключаем саму библиотеку LVGL
add_subdirectory(lvgl)

# 3. Исходники обертки симулятора (оставляем только SDL)
file(GLOB LV_LINUX_SRC src/lib/*.c)
set(LV_LINUX_BACKEND_SRC src/lib/display_backends/sdl.c src/lib/indev_backends/evdev.c)

add_library(lvgl_linux ${LV_LINUX_SRC} ${LV_LINUX_BACKEND_SRC})
target_link_libraries(lvgl_linux PUBLIC lvgl::lvgl)

# ДОБАВЛЕНО: Ищем и линкуем системную библиотеку SDL2
find_package(SDL2 REQUIRED)
target_include_directories(lvgl_linux PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(lvgl_linux PUBLIC ${SDL2_LIBRARIES})

# 4. СОБИРАЕМ ВАШ ПРОЕКТ
set(MY_UI_SOURCES 
    src/main.c
    # Сюда потом добавите ваши файлы: ../main/config/project_config.c и т.д.
)

add_executable(lvglsim ${MY_UI_SOURCES})

# Линкуем LVGL и обертку симулятора
target_link_libraries(lvglsim PUBLIC lvgl_linux)