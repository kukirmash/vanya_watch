#ifndef VW_TIME_H
#define VW_TIME_H

#include <time.h>

//-----------------------------------------------------------------------------------------
/**
 * @brief Получить текущее системное время
 * @param timeinfo Указатель на структуру tm, куда будет записано время
 */
void get_curr_time(struct tm *timeinfo);

//-----------------------------------------------------------------------------------------
/**
 * @brief Установить текущее системное время в RTC
 * @param year Год (например, 2026)
 * @param month Месяц (1 - 12)
 * @param day День месяца (1 - 31)
 * @param hour Часы (0 - 23)
 * @param minute Минуты (0 - 59)
 * @param second Секунды (0 - 59)
 */
void set_curr_time(int year, int month, int day, int hour, int minute, int second);

//-----------------------------------------------------------------------------------------

#endif // VW_TIME_H