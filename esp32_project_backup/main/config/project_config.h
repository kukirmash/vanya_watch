#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

#include "module_config.h"
#include "lvgl_lcd_conf.h"

#define VW_LCD_H_RES (240)
#define VW_LCD_V_RES (280)
#define VW_LCD_DRAW_BUFF_DOUBLE (1)
#define VW_LCD_DRAW_BUFF_HEIGHT (VW_LCD_V_RES / 4)

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
// Возможные состояния Wi-Fi модуля
typedef enum {
	WIFI_STATE_DISABLED = 0, // Wi-Fi физически выключен
	WIFI_STATE_DISCONNECTED, // Включен, нет сети (идет сканирование)
	WIFI_STATE_CONNECTING,   // В процессе подключения (блокирует сканирование)
	WIFI_STATE_CONNECTED,     // Подключен (можно скачивать погоду и время)
	WIFI_STATE_BUSY
} pc_wifi_state_t;

//-----------------------------------------------------------------------------------------
typedef struct {
	char ssid[33];
	int8_t rssi;
	bool is_secure;
} pc_ap_info;

//-----------------------------------------------------------------------------------------
typedef struct {
	pc_wifi_state_t state;

	pc_ap_info ap_list[15];
	uint8_t ap_count;

	// Указатель на текущую подключенную сеть. Если NULL - сети нет!
	pc_ap_info* connected_ap;

	char password[65]; // Пароль от последней/текущей сети
	char ip_address[16];

	char icon_str[8];
} pc_wifi_config;

//-----------------------------------------------------------------------------------------
typedef struct {
	bool is_valid;         // Успешно ли получены данные
	int8_t temp;           // Температура (в градусах Цельсия)
	char icon[8];          // Код иконки (например "01d" для ясного неба)
	char description[32];  // Текстовое описание ("clear sky", "light rain")
} pc_weather_config;

//-----------------------------------------------------------------------------------------
// Глобальная структура
typedef struct {
	pc_time_config time;
	pc_power_config power;
	pc_wifi_config wifi;
	pc_weather_config weather;
} project_config;

//-----------------------------------------------------------------------------------------
// Глобальные Subjects (Наблюдаемые объекты)
extern lv_subject_t subject_time;
extern lv_subject_t subject_power;
extern lv_subject_t subject_wifi;
extern lv_subject_t subject_weather;

extern lv_subject_t subject_time_str;
extern lv_subject_t subject_date_str;
extern lv_subject_t subject_power_str;
extern lv_subject_t subject_wifi_str;

//-----------------------------------------------------------------------------------------
//Singleton
void project_config_init( void );

project_config* project_config_get( void );

// Блокирует конфиг для безопасного чтения из фоновых задач
void project_config_lock( void );
void project_config_unlock( void );

//-----------------------------------------------------------------------------------------
// Сеттеры (для записи из рабочих потоков)
void project_config_set_time( uint8_t h, uint8_t m, uint8_t s, uint8_t d, uint8_t mo, uint16_t y );

void project_config_set_power( uint8_t percent, uint32_t mv, bool charging );

void project_config_set_wifi_state( pc_wifi_state_t state );
void project_config_set_wifi_ap_list( const pc_ap_info* list, uint8_t count );
void project_config_set_wifi_connected_ap( const pc_ap_info* ap, const char* ip );
void project_config_set_wifi_password( const char* pwd );

void project_config_set_weather( int8_t temp, const char* icon, const char* desc );

//-----------------------------------------------------------------------------------------
// Геттеры (для чтения из UI / LVGL потока)
void project_config_get_time( pc_time_config* out_time );
void project_config_get_power( pc_power_config* out_power );
void project_config_get_wifi( pc_wifi_config* out_wifi );
void project_config_get_weather( pc_weather_config* out_weather );

//-----------------------------------------------------------------------------------------

#endif // PROJECT_CONFIG_H