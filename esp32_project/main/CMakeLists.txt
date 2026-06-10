file(GLOB image_frames "project_lcd/images/watchface2/*.c")

idf_component_register(
    SRCS 
        "main.c" 
        "board/lcd.c"
        "board/touch.c"
        "board/power.c"
        "board/time.c"
        "board/wifi.c"

        "config/project_config.c"

        "project_lcd/control_center.c"
        "project_lcd/demo_sequential.c"
        "project_lcd/project_lcd_main.c"
        "project_lcd/watchface.c"
        "project_lcd/wifi_window.c"

        "project_lcd/fonts/science_gothic/science_gothic_14.c"
        "project_lcd/fonts/science_gothic/science_gothic_18.c"
        "project_lcd/fonts/science_gothic/science_gothic_22.c"
        
        ${image_frames}
    INCLUDE_DIRS 
        "." 
        "project_lcd"
        "config"

    REQUIRES
        esp_adc
        esp_wifi
        nvs_flash
        esp_netif
)

target_compile_definitions(${COMPONENT_LIB} PRIVATE LV_LVGL_H_INCLUDE_SIMPLE)