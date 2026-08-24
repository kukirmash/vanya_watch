#include "board/time.h"

#include "helpers/thread/thread_helper.h"

//-----------------------------------------------------------------------------------------
lv_subject_t subject_time;
static struct tm current_timeinfo;

//-----------------------------------------------------------------------------------------
static void time_update_task(void *arg)
{
    while (1)
    {
        time_t now;
        time(&now);

        // Передаем UNIX-время в виде обычного числа. Оно каждую секунду разное
        lv_lock();
        lv_subject_set_int(&subject_time, (int32_t)now);
        lv_unlock();

        thread_sleep_ms(1000);
    }
}

//-----------------------------------------------------------------------------------------
void board_time_init(void)
{
    lv_subject_init_int(&subject_time, 0);
    thread_create(time_update_task, NULL, 4096, 5, "time_task");
}

//-----------------------------------------------------------------------------------------