#include "board/power.h"
#include "board/lcd.h"
#include "board/touch.h"
#include "board/time.h"
#include "board/wifi.h"
#include "project_lcd/project_lcd_main.h"
#include "config/project_config.h"

//-----------------------------------------------------------------------------------------
void app_main(void)
{
    project_config_init();

    // Инициализируем питание
    power_init();

    // Дисплей и тач
    esp_lcd_panel_io_handle_t lcd_io = NULL;
    esp_lcd_panel_handle_t lcd_panel = NULL;
    esp_lcd_touch_handle_t touch_handle = NULL;
    ESP_ERROR_CHECK(lcd_app_init(&lcd_io, &lcd_panel));
    ESP_ERROR_CHECK(app_touch_init(&touch_handle));

    // 1. СНАЧАЛА инициализируем порт LVGL (создаются мьютексы)
    ESP_ERROR_CHECK(app_lvgl_init(lcd_io, lcd_panel, touch_handle));

    // 2. ТЕПЕРЬ безопасно запускаем модули. time_init запишет время в конфиг, 
    // и мьютекс lvgl_port_lock(0) внутри конфига успешно сработает!
    time_init();
    wifi_init();
    
    // 3. И ТОЛЬКО ПОТОМ рисуем графику. Циферблат сразу увидит правильное время!
    app_main_display();
}

//-----------------------------------------------------------------------------------------
