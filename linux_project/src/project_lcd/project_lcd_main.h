#ifndef PROJECT_LCD_MAIN_H
#define PROJECT_LCD_MAIN_H

#include "config/modules_config.h"

#if MOD_LVGL_LCD

//-----------------------------------------------------------------------------------------
// Функция запуска 
void lvgl_lcd(void); 

//-----------------------------------------------------------------------------------------

#if ESP32

#include "esp_err.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_touch_cst816s.h"

//-----------------------------------------------------------------------------------------
// Функция для инициализации LVGL ESP32
esp_err_t app_lvgl_init(esp_lcd_panel_io_handle_t lcd_io, esp_lcd_panel_handle_t lcd_panel, esp_lcd_touch_handle_t touch_handle);

//-----------------------------------------------------------------------------------------
// 
void app_main_display(void);

#endif // ESP32

//-----------------------------------------------------------------------------------------

#endif // MOD_LVGL_LCD
#endif // PROJECT_LCD_MAIN_H
