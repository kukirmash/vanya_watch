#ifndef PROJECT_LCD_MAIN_H
#define PROJECT_LCD_MAIN_H

#include "lvgl_lcd_conf.h"

#include "esp_err.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_touch_cst816s.h"

//-----------------------------------------------------------------------------------------
// Функция для инициализации LVGL
esp_err_t app_lvgl_init(esp_lcd_panel_io_handle_t lcd_io, esp_lcd_panel_handle_t lcd_panel, esp_lcd_touch_handle_t touch_handle);

//-----------------------------------------------------------------------------------------
// Запуск экрана
void app_main_display(void);

//-----------------------------------------------------------------------------------------
void lvgl_lcd(void); // Ваша существующая функция запуска графики

//-----------------------------------------------------------------------------------------

#endif
