#ifndef DEMO_SEQUENTIAL_H
#define DEMO_SEQUENTIAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl_lcd_conf.h"

#if MOD_LVGL_LCD && VW_DEMO_SEQUENTIAL_ANIMATION
    //-----------------------------------------------------------------------------------------
    void demo_sequential_init( void );

    //-----------------------------------------------------------------------------------------
#endif

#ifdef __cplusplus
}
#endif // extern "C"

#endif // DEMO_SEQUENTIAL_H
