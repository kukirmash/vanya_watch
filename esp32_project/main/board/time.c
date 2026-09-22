#include "board/time.h"

#include "config/modules_config.h"
#include "helpers/thread/thread_helper.h"

#include <stdio.h>
#include <string.h>

#if ESP32
#include <sys/time.h>

#include "esp_log.h"
#include "esp_lvgl_port.h"
static const char* TAG = "BOARD_TIME";
#endif

//-----------------------------------------------------------------------------------------
// Объекты LVGL. subject_time хранит Unix timestamp, subject_str_time - строку "HH:MM".
lv_subject_t subject_time;
lv_subject_t subject_str_time;

static char time_str_buffer[6];      // Буфер для строки времени "HH:MM"
static char prev_time_str_buffer[6]; // Предыдущее значение строки (требование LVGL)

//-----------------------------------------------------------------------------------------
#if ESP32
//-----------------------------------------------------------------------------------------
// Установка системного времени из момента компиляции.
// Работает до первой синхронизации по SNTP.
static void set_time_from_compile(void)
{
	char s_month[5];
	int year, day, hour, minute, second;
	int month = 1;

	// Парсим строку даты, например: "May 26 2026"
	sscanf(__DATE__, "%s %d %d", s_month, &day, &year);
	// Парсим строку времени, например: "16:05:00"
	sscanf(__TIME__, "%d:%d:%d", &hour, &minute, &second);

	// Переводим текстовое название месяца в число (1-12)
	const char* month_names = "JanFebMarAprMayJunJulAugSepOctNovDec";
	const char* m_pos = strstr(month_names, s_month);
	if (m_pos != NULL)
		month = (int)(m_pos - month_names) / 3 + 1;

	struct tm t = { 0 };
	t.tm_year = year - 1900;
	t.tm_mon = month - 1;
	t.tm_mday = day;
	t.tm_hour = hour;
	t.tm_min = minute;
	t.tm_sec = second;
	t.tm_isdst = -1;

	struct timeval tv = { 0 };
	tv.tv_sec = mktime(&t);
	tv.tv_usec = 0;
	settimeofday(&tv, NULL);

	ESP_LOGI(TAG, "Time set from compiler: %02d.%02d.%04d %02d:%02d:%02d",
		day, month, year, hour, minute, second);
}
#endif // ESP32

//-----------------------------------------------------------------------------------------
static void time_update_task(void* arg)
{
	while (1)
	{
		time_t now = 0;
		time(&now);

		struct tm timeinfo;
#if LINUX
		struct tm* tmp = localtime(&now);
		timeinfo = *tmp;
#else
		localtime_r(&now, &timeinfo);
#endif

		char temp_str_buffer[6];
		snprintf(temp_str_buffer, sizeof(temp_str_buffer), "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);

#if ESP32
		lvgl_port_lock(0);
#endif
		lv_subject_set_int(&subject_time, (int32_t)now);
		lv_subject_copy_string(&subject_str_time, temp_str_buffer);
#if ESP32
		lvgl_port_unlock();
#endif

		thread_sleep_ms(1000);
	}
}

//-----------------------------------------------------------------------------------------
void board_time_init(void)
{
#if ESP32
	// До подключения к сети выставляем время из момента компиляции.
	// После SNTP-синхронизации системное время обновится автоматически.
	set_time_from_compile();
#endif

	lv_subject_init_int(&subject_time, 0);
	lv_subject_init_string(&subject_str_time, time_str_buffer, prev_time_str_buffer, sizeof(time_str_buffer), "00:00");

	thread_create(time_update_task, NULL, 4096, 5, "time_thread");
}

//-----------------------------------------------------------------------------------------
