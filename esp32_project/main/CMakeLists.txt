# Собираем все исходники новых окон/анимаций/картинок/шрифтов и хелперов
file(GLOB_RECURSE app_ui_sources "app_ui/*.c")
file(GLOB_RECURSE helper_sources "helpers/*.c")

idf_component_register(
    SRCS
        "main.c"
        "board/lcd.c"
        "board/touch.c"
        "board/power.c"
        "board/time.c"
        "board/wifi.c"

        ${app_ui_sources}
        ${helper_sources}

    INCLUDE_DIRS
        "."

    REQUIRES
        esp_adc
        esp_wifi
        nvs_flash
        esp_netif
        esp_event
        esp_hw_support
        esp_driver_spi
        esp_driver_gpio
        esp_driver_i2c
        esp_lcd
)

target_compile_definitions(${COMPONENT_LIB} PRIVATE LV_LVGL_H_INCLUDE_SIMPLE)
