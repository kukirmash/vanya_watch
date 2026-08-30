#ifndef UI_CONFIG_H
#define UI_CONFIG_H

#include "lvgl.h"

//-----------------------------------------------------------------------------------------
// Цвета
#define VW_PRIMARY_COLOR_HEX        0xFEB563
#define VW_SECONDARY_COLOR_HEX      0x838383

#define VW_GREY_COLOR_HEX           0x8E8E8E

//-----------------------------------------------------------------------------------------
// Шрифты
LV_FONT_DECLARE( science_gothic_14 ); 
LV_FONT_DECLARE( science_gothic_18 ); 
LV_FONT_DECLARE( science_gothic_22 );

#define VW_FONT_14                  &science_gothic_14
#define VW_FONT_18                  &science_gothic_18
#define VW_FONT_22                  &science_gothic_22

//-----------------------------------------------------------------------------------------
// Cимволы
#define VW_SYMBOL_WIFI_LOW      	"\xEF\x9A\xAA"
#define VW_SYMBOL_WIFI_MID      	"\xEF\x9A\xAB"
#define VW_SYMBOL_BIRGHTNESS    	"\xEE\x83\x89"
#define VW_SYMBOL_LOCK				"\xEF\x80\xA3"

//-----------------------------------------------------------------------------------------
// Глубина меню
#define MAX_MENU_DEPTH 				8

//-----------------------------------------------------------------------------------------

#endif // UI_CONFIG_H