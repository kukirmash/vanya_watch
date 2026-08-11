#ifndef TOUCH_H
#define TOUCH_H

#include "esp_err.h"
#include "esp_lcd_touch_cst816s.h"

//-----------------------------------------------------------------------------------------
esp_err_t app_touch_init(esp_lcd_touch_handle_t *ret_touch);

//-----------------------------------------------------------------------------------------

#endif // TOUCH_H
