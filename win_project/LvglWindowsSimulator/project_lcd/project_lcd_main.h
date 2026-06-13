#ifndef PROJECT_LCD_MAIN_H
#define PROJECT_LCD_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl_lcd_conf.h"

#if MOD_LVGL_LCD

//-----------------------------------------------------------------------------------------
// Функция запуска графики
void lvgl_lcd();

//-----------------------------------------------------------------------------------------

#ifdef __cplusplus
} 
#endif //  extern "C"

#endif // MOD_LVGL_LCD
#endif // PROJECT_LCD_MAIN_H
