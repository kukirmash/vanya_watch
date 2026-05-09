#ifndef LVGL_LCD_CONF
#define LVGL_LCD_CONF

//-----------------------------------------------------------------------------------------
#include "config/module_config.h"
#include "lvgl.h"

LV_FONT_DECLARE(science_gothic_14); LV_FONT_DECLARE(science_gothic_18); LV_FONT_DECLARE(science_gothic_22);

//-----------------------------------------------------------------------------------------
// Режимы работы
#define VW_DEMO_SEQUENTIAL_ANIMATION    0
#define VW_WORK_MODE                    1

//-----------------------------------------------------------------------------------------
// Цвета
#define VW_PRIMARY_COLOR_HEX                  0xFEB563
#define VW_SECONDARY_COLOR_HEX                0x838383

//-----------------------------------------------------------------------------------------
// Шрифты
#define VW_FONT_14                    &science_gothic_14
#define VW_FONT_18                    &science_gothic_18
#define VW_FONT_22                    &science_gothic_22

//-----------------------------------------------------------------------------------------
// Cимволы
#define VW_SYMBOL_WIFI_LOW                         "\xEF\x9A\xAA"
#define VW_SYMBOL_WIFI_MID                         "\xEF\x9A\xAB"
#define VW_SYMBOL_BIRGHTNESS                       "\xEE\x83\x89"

//-----------------------------------------------------------------------------------------


#endif // LVGL_LCD_CONF