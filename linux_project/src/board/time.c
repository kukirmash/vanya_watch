#include "board/time.h"

#include "helpers/thread/thread_helper.h"

//-----------------------------------------------------------------------------------------
lv_subject_t subject_time;
lv_subject_t subject_str_time;

static char time_str_buffer[6];         // Буфер для строки времени "HH:MM"
static char prev_time_str_buffer[6];    // Буфер для строки времени "HH:MM"

//-----------------------------------------------------------------------------------------
static void time_update_task(void *arg)
{
    while (1)
    {
        time_t now;
        time(&now);
        struct tm *timeinfo = localtime(&now);

        lv_lock();
        char temp_str_buffer[6];
        lv_snprintf(temp_str_buffer, sizeof(temp_str_buffer), "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
    
        lv_subject_set_int(&subject_time, (int32_t)now);
        lv_subject_copy_string(&subject_str_time, temp_str_buffer);
        lv_unlock();

        thread_sleep_ms(1000);
    }
}

//-----------------------------------------------------------------------------------------
void board_time_init(void)
{
    lv_subject_init_int(&subject_time, 0);
    lv_subject_init_string(&subject_str_time, time_str_buffer, prev_time_str_buffer, sizeof(time_str_buffer), "00:00");
    thread_create(time_update_task, NULL, 4096, 5, "time_task");
}

//-----------------------------------------------------------------------------------------