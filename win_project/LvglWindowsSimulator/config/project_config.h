#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "module_config.h"
#include "../project_lcd/lvgl_lcd_conf.h"

#define VW_LCD_H_RES (240)
#define VW_LCD_V_RES (280)

#include <stdint.h>
#include <stdbool.h>

//-----------------------------------------------------------------------------------------
typedef struct {
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint8_t day;
	uint8_t month;
	uint16_t year;

	char time_str[8];
	char date_str[32];
} pc_time_config;

//-----------------------------------------------------------------------------------------
typedef struct {
	uint8_t battery_percent;
	uint32_t battery_mvolt;
	bool is_charging;

	char power_str[16];
} pc_power_config;

//-----------------------------------------------------------------------------------------
typedef struct {
	bool is_enabled;
	bool is_connected;
	char ssid[32];
    char password[64];
	char ip_address[16];
	int8_t rssi;
	bool is_secure;
} pc_wifi_config; // TODO: хранить сразу wifi_ap_record_t

//-----------------------------------------------------------------------------------------
// Глобальная структура
typedef struct {
	pc_time_config time;
	pc_power_config power;
	pc_wifi_config wifi;
} project_config;

//-----------------------------------------------------------------------------------------
// Глобальные Subjects (Наблюдаемые объекты)
extern lv_subject_t subject_time;
extern lv_subject_t subject_power;
extern lv_subject_t subject_wifi;
extern lv_subject_t subject_time_str;
extern lv_subject_t subject_date_str;
extern lv_subject_t subject_power_str;

//-----------------------------------------------------------------------------------------
//Singleton
void project_config_init( void );

//-----------------------------------------------------------------------------------------
// Сеттеры (для записи из рабочих потоков)
void project_config_set_time( uint8_t h, uint8_t m, uint8_t s, uint8_t d, uint8_t mo, uint16_t y );
void project_config_set_power( uint8_t percent, uint32_t mv, bool charging );
void project_config_set_wifi_status( bool connected, const char* ssid, const char* ip, int8_t rssi, bool is_secure );
void project_config_set_wifi_enabled( bool enabled );

//-----------------------------------------------------------------------------------------
// Геттеры (для чтения из UI / LVGL потока)
void project_config_get_time( pc_time_config* out_time );
void project_config_get_power( pc_power_config* out_power );
void project_config_get_wifi( pc_wifi_config* out_wifi );

#ifdef __cplusplus
}
#endif //  extern "C"

#endif // PROJECT_CONFIG_H
