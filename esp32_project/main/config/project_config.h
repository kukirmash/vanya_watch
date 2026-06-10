#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

#include "module_config.h"
#include "lvgl_lcd_conf.h"

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
} config_time_t;

//-----------------------------------------------------------------------------------------
typedef struct {
    uint8_t battery_percent;
    uint32_t battery_mvolt;
    bool is_charging;

    char power_str[16];
} config_power_t;

//-----------------------------------------------------------------------------------------
typedef struct {
    bool is_enabled;
    bool is_connected;
    char ssid[32];
    char ip_address[16];
} config_wifi_t;

//-----------------------------------------------------------------------------------------
// Глобальная структура
typedef struct {
    config_time_t time;
    config_power_t power;
    config_wifi_t wifi;
} project_config_t;

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
void project_config_init(void);

//-----------------------------------------------------------------------------------------
// Сеттеры (для записи из рабочих потоков)
void project_config_set_time(uint8_t h, uint8_t m, uint8_t s, uint8_t d, uint8_t mo, uint16_t y);
void project_config_set_power(uint8_t percent, uint32_t mv, bool charging);
void project_config_set_wifi_status(bool connected, const char* ssid, const char* ip);

//-----------------------------------------------------------------------------------------
// Геттеры (для чтения из UI / LVGL потока)
// Мы передаем указатель, куда скопировать безопасные данные
void project_config_get_time(config_time_t* out_time);
void project_config_get_power(config_power_t* out_power);
void project_config_get_wifi(config_wifi_t* out_wifi);

#endif // PROJECT_CONFIG_H