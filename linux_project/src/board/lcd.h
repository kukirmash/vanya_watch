#ifndef LCD_H
#define LCD_H

#include "esp_err.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"

//-----------------------------------------------------------------------------------------
// Вернет готовые указатели на IO и Panel
esp_err_t lcd_app_init( esp_lcd_panel_io_handle_t* ret_io, esp_lcd_panel_handle_t* ret_panel );

//-----------------------------------------------------------------------------------------
void lcd_set_power_state( bool on );

//-----------------------------------------------------------------------------------------

#endif // LCD_H
