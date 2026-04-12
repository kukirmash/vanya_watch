#ifndef WATCHFACE_H
#define WATCHFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl_lcd_conf.h"

#if MOD_LVGL_LCD && VW_WORK_MODE

    //-----------------------------------------------------------------------------------------
    void watchface_init( lv_obj_t* tile );

    //-----------------------------------------------------------------------------------------
#endif // MOD_LVGL_LCD && VW_WORK_MODE

#ifdef __cplusplus
}
#endif //  extern "C"

#endif // WATCHFACE_H
