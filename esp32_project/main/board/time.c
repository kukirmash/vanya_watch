#include "time.h"
#include <sys/time.h>

#include "config/module_config.h"

//-----------------------------------------------------------------------------------------
void get_curr_time(struct tm *timeinfo)
{
    if (timeinfo == NULL) {
        return;
    }

    time_t now;
    time(&now);

#if WINDOWS
    localtime_s(timeinfo, &now);
#elif ESP32
    localtime_r(&now, timeinfo);
#endif
}

//-----------------------------------------------------------------------------------------
void set_curr_time(int year, int month, int day, int hour, int minute, int second)
{
    struct tm t;
    
    // Приведение к стандарту Си: год отсчитывается с 1900, месяцы с 0
    t.tm_year = year - 1900; 
    t.tm_mon = month - 1;    
    t.tm_mday = day;
    t.tm_hour = hour;
    t.tm_min = minute;
    t.tm_sec = second;
    t.tm_isdst = -1; // Автоматическое определение летнего времени (если применимо)

    // Преобразование структуры tm в секунды Unix Epoch
    time_t time_since_epoch = mktime(&t);
    
    struct timeval tv;
    tv.tv_sec = time_since_epoch;
    tv.tv_usec = 0;
    
    // Запись времени непосредственно в аппаратный RTC микроконтроллера
    settimeofday(&tv, NULL);
}
//-----------------------------------------------------------------------------------------

