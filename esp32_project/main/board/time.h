#ifndef BOARD_TIME_H
#define BOARD_TIME_H

#include "lvgl.h"
#include <time.h>

//-----------------------------------------------------------------------------------------
// Субъекты времени:
// subject_time     - Unix timestamp (int)
// subject_str_time - строка "HH:MM"
extern lv_subject_t subject_time;
extern lv_subject_t subject_str_time;

//-----------------------------------------------------------------------------------------
void board_time_init(void);

//-----------------------------------------------------------------------------------------

#endif // BOARD_TIME_H
