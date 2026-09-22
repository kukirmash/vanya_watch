#ifndef CONTROL_CENTER_H
#define CONTROL_CENTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl_lcd_conf.h"

#if MOD_LVGL_LCD && VW_WORK_MODE
    //-----------------------------------------------------------------------------------------
    void control_center_init( lv_obj_t* parent );

    //-----------------------------------------------------------------------------------------
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // CONTROL_CENTER_H
