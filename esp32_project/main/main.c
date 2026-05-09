#include "board/power.h"
#include "board/lcd.h"
#include "board/touch.h"
#include "project_lcd/project_lcd_main.h"

//-----------------------------------------------------------------------------------------
void app_main(void)
{
    // 1. Инициализируем питание (удержание, кнопка, батарея)
    power_init();

    // 2. Железо (дисплей, тач)
    esp_lcd_panel_io_handle_t lcd_io = NULL;
    esp_lcd_panel_handle_t lcd_panel = NULL;
    esp_lcd_touch_handle_t touch_handle = NULL;

    ESP_ERROR_CHECK(app_lcd_init(&lcd_io, &lcd_panel));
    ESP_ERROR_CHECK(app_touch_init(&touch_handle));
    
    // 3. Графика (lvgl)
    ESP_ERROR_CHECK(app_lvgl_init(lcd_io, lcd_panel, touch_handle));
    app_main_display();
}

//-----------------------------------------------------------------------------------------
