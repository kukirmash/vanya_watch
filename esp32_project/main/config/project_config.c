#include "project_config.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_lvgl_port.h"

#include <string.h>

//-----------------------------------------------------------------------------------------
// Статическая переменная - это и есть наш Singleton
static project_config s_config;

// Объекты Subjects LVGL 9
lv_subject_t subject_time;
lv_subject_t subject_power;
lv_subject_t subject_wifi;
lv_subject_t subject_time_str;
lv_subject_t subject_date_str;
lv_subject_t subject_power_str;

static const char* month_names[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December" };

//-----------------------------------------------------------------------------------------
void project_config_init( void )
{
    memset( &s_config, 0, sizeof( project_config ) );
    s_config.wifi.is_enabled = true; // По умолчанию включен

    // Инициализируем Observer'ы (передаем указатели на соответствующие структуры)
    lv_subject_init_pointer( &subject_time, &s_config.time );
    lv_subject_init_pointer( &subject_power, &s_config.power );
    lv_subject_init_pointer( &subject_wifi, &s_config.wifi );

    lv_subject_init_string( &subject_time_str, s_config.time.time_str, NULL, sizeof( s_config.time.time_str ), "--:--" );
    lv_subject_init_string( &subject_date_str, s_config.time.date_str, NULL, sizeof( s_config.time.date_str ), "-- ---" );
    lv_subject_init_string( &subject_power_str, s_config.power.power_str, NULL, sizeof( s_config.power.power_str ), "" );
}

//-----------------------------------------------------------------------------------------
void project_config_set_time( uint8_t h, uint8_t m, uint8_t s, uint8_t d, uint8_t mo, uint16_t y )
{
    lvgl_port_lock( 0 ); // Защищаем обновление LVGL

    if ( s_config.time.hour != h || s_config.time.minute != m || s_config.time.second != s ||
        s_config.time.day != d || s_config.time.month != mo || s_config.time.year != y )
    {
        s_config.time.hour = h;
        s_config.time.minute = m;
        s_config.time.second = s;
        s_config.time.day = d;
        s_config.time.month = mo;
        s_config.time.year = y;

        char temp_time[8];
        snprintf( temp_time, sizeof( temp_time ), "%02d:%02d", h, m );
        lv_subject_copy_string( &subject_time_str, temp_time );

        uint8_t mon_idx = ( mo > 0 && mo <= 12 ) ? mo - 1 : 0;
        char temp_date[32];
        snprintf( temp_date, sizeof( temp_date ), "%d %s", d, month_names[mon_idx] );
        lv_subject_copy_string( &subject_date_str, temp_date );

        // Уведомляем тех, кто подписан на структуру целиком (для анимаций часов)
        lv_subject_set_pointer( &subject_time, &s_config.time );
    }

    lvgl_port_unlock();
}

//-----------------------------------------------------------------------------------------
void project_config_set_power( uint8_t percent, uint32_t mv, bool charging )
{
    lvgl_port_lock( 0 );

    bool ui_needs_update = ( s_config.power.battery_percent != percent ||
        s_config.power.is_charging != charging );

    s_config.power.battery_percent = percent;
    s_config.power.battery_mvolt = mv;
    s_config.power.is_charging = charging;

    if ( ui_needs_update )
    {
        const char* battery_symbols[5] = {
            LV_SYMBOL_BATTERY_EMPTY, LV_SYMBOL_BATTERY_1, LV_SYMBOL_BATTERY_2,
            LV_SYMBOL_BATTERY_3, LV_SYMBOL_BATTERY_FULL };
        int index = percent / 20;
        if ( index > 4 ) index = 4;
        const char* battery_symbol = battery_symbols[index];

        char temp_power[16];
        if ( charging )
            snprintf( temp_power, sizeof( temp_power ), LV_SYMBOL_CHARGE " %s %d%%", battery_symbol, percent );
        else
            snprintf( temp_power, sizeof( temp_power ), "%s %d%%", battery_symbol, percent );

        lv_subject_copy_string( &subject_power_str, temp_power );
        lv_subject_set_pointer( &subject_power, &s_config.power );
    }

    lvgl_port_unlock();
}

//-----------------------------------------------------------------------------------------
void project_config_set_wifi_status( bool connected, const char* ssid, const char* ip )
{
    lvgl_port_lock( 0 );

    const char* safe_ssid = ssid ? ssid : "";
    const char* safe_ip = ip ? ip : "";

    bool changed =
        ( s_config.wifi.is_connected != connected ) ||
        ( strncmp( s_config.wifi.ssid, safe_ssid, sizeof( s_config.wifi.ssid ) ) != 0 ) ||
        ( strncmp( s_config.wifi.ip_address, safe_ip, sizeof( s_config.wifi.ip_address ) ) != 0 );

    if ( changed )
    {
        s_config.wifi.is_connected = connected;

        strncpy( s_config.wifi.ssid, safe_ssid, sizeof( s_config.wifi.ssid ) - 1 );
        s_config.wifi.ssid[sizeof( s_config.wifi.ssid ) - 1] = '\0'; // Гарантируем нуль-терминатор

        strncpy( s_config.wifi.ip_address, safe_ip, sizeof( s_config.wifi.ip_address ) - 1 );
        s_config.wifi.ip_address[sizeof( s_config.wifi.ip_address ) - 1] = '\0';

        lv_subject_set_pointer( &subject_wifi, &s_config.wifi );
    }

    lvgl_port_unlock();
}

//-----------------------------------------------------------------------------------------
void project_config_set_wifi_enabled( bool enabled )
{
    lvgl_port_lock( 0 );

    if ( s_config.wifi.is_enabled != enabled )
    {
        s_config.wifi.is_enabled = enabled;
        lv_subject_set_pointer( &subject_wifi, &s_config.wifi );
    }

    lvgl_port_unlock();
}

//-----------------------------------------------------------------------------------------
// Геттеры
void project_config_get_power( pc_power_config* out_power )
{
    if ( !out_power )
        return;
    lvgl_port_lock( 0 );
    *out_power = s_config.power; // Копируем структуру целиком
    lvgl_port_unlock();
}

//-----------------------------------------------------------------------------------------
void project_config_get_time( pc_time_config* out_time )
{
    if ( !out_time )
        return;
    lvgl_port_lock( 0 );
    *out_time = s_config.time;
    lvgl_port_unlock();
}

//-----------------------------------------------------------------------------------------
void project_config_get_wifi( pc_wifi_config* out_wifi )
{
    if ( !out_wifi )
        return;
    lvgl_port_lock( 0 );
    *out_wifi = s_config.wifi;
    lvgl_port_unlock();
}

//-----------------------------------------------------------------------------------------