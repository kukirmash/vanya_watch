#ifndef VW_WIFI_WINDOW_H
#define VW_WIFI_WINDOW_H

#include "lvgl_lcd_conf.h"

#if MOD_LVGL_LCD && VW_WORK_MODE

//-----------------------------------------------------------------------------------------
/**
 * @brief Создает и отображает окно настроек Wi-Fi
 */
void wifi_window_create( void );

//-----------------------------------------------------------------------------------------
#endif // MOD_LVGL_LCD && VW_WORK_MODE
#endif // VW_WIFI_WINDOW_H
