#ifndef LCD_H
#define LCD_H

#include "esp_err.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"

/* понадобится для LVGL и Touch */
#define VW_LCD_H_RES (240)
#define VW_LCD_V_RES (280)
#define VW_LCD_DRAW_BUFF_DOUBLE (1)
#define VW_LCD_DRAW_BUFF_HEIGHT (VW_LCD_V_RES / 2)

//-----------------------------------------------------------------------------------------
// Вернет готовые указатели на IO и Panel
esp_err_t lcd_app_init(esp_lcd_panel_io_handle_t *ret_io, esp_lcd_panel_handle_t *ret_panel);

//-----------------------------------------------------------------------------------------
void lcd_set_power_state(bool on);

//-----------------------------------------------------------------------------------------

#endif // LCD_H
