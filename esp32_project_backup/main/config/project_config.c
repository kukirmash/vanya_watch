#include "project_config.h"

#if ESP32
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_lvgl_port.h"
#endif // ESP32

#include <string.h>

//-----------------------------------------------------------------------------------------
// Статическая переменная - это и есть наш Singleton
static project_config* pc_config = NULL;

//-----------------------------------------------------------------------------------------
// Объекты Subjects LVGL 
lv_subject_t subject_time;
lv_subject_t subject_power;
lv_subject_t subject_wifi;
lv_subject_t subject_weather;
lv_subject_t subject_time_str;
lv_subject_t subject_date_str;
lv_subject_t subject_power_str;
lv_subject_t subject_wifi_str;

//-----------------------------------------------------------------------------------------
void project_config_init( void )
{
	pc_config = ( project_config* )malloc( sizeof( project_config ) );
	memset( pc_config, 0, sizeof( project_config ) );

	// 2. Инициализируем Observer'ы
	lv_subject_init_pointer( &subject_time, &pc_config->time );
	lv_subject_init_pointer( &subject_power, &pc_config->power );
	lv_subject_init_pointer( &subject_wifi, &pc_config->wifi );
	lv_subject_init_pointer( &subject_weather, &pc_config->weather );

	lv_subject_init_string( &subject_time_str, pc_config->time.time_str, NULL, sizeof( pc_config->time.time_str ), "--:--" );
	lv_subject_init_string( &subject_date_str, pc_config->time.date_str, NULL, sizeof( pc_config->time.date_str ), "-- ---" );
	lv_subject_init_string( &subject_power_str, pc_config->power.power_str, NULL, sizeof( pc_config->power.power_str ), "" );
	lv_subject_init_string( &subject_wifi_str, pc_config->wifi.icon_str, NULL, sizeof( pc_config->wifi.icon_str ), "" );
}

//-----------------------------------------------------------------------------------------
project_config* project_config_get( void )
{
	return pc_config;
}

//-----------------------------------------------------------------------------------------
void project_config_lock( void )
{
	lvgl_port_lock( 0 );
}

//-----------------------------------------------------------------------------------------
void project_config_unlock( void )
{
	lvgl_port_unlock();
}

//-----------------------------------------------------------------------------------------
void project_config_set_time( uint8_t h, uint8_t m, uint8_t s, uint8_t d, uint8_t mo, uint16_t y )
{
	lvgl_port_lock( 0 ); // Защищаем обновление LVGL

	pc_time_config* curr_time = &pc_config->time;

	if ( curr_time->hour != h || curr_time->minute != m || curr_time->second != s || curr_time->day != d || curr_time->month != mo || curr_time->year != y )
	{
		// Обновляем время в конфиге
		curr_time->hour = h;
		curr_time->minute = m;
		curr_time->second = s;
		curr_time->day = d;
		curr_time->month = mo;
		curr_time->year = y;

		// Время
		char temp_time_buf[8];
		snprintf( temp_time_buf, sizeof( temp_time_buf ), "%02d:%02d", h, m );

		// Дата
		const char* month_names[] = {
		"January", "February", "March", "April", "May", "June",
		"July", "August", "September", "October", "November", "December" };
		uint8_t mon_idx = ( mo > 0 && mo <= 12 ) ? mo - 1 : 0;
		char temp_date_buf[32];
		snprintf( temp_date_buf, sizeof( temp_date_buf ), "%d %s", d, month_names[mon_idx] );

		// Обновляем subject
		lv_subject_copy_string( &subject_time_str, temp_time_buf );
		lv_subject_copy_string( &subject_date_str, temp_date_buf );
		lv_subject_set_pointer( &subject_time, curr_time );
	}

	lvgl_port_unlock();
}

//-----------------------------------------------------------------------------------------
void project_config_set_power( uint8_t percent, uint32_t mv, bool charging )
{
	lvgl_port_lock( 0 );

	pc_power_config* curr_power = &pc_config->power;

	if ( curr_power->battery_percent != percent || curr_power->is_charging != charging )
	{
		// Обновляем конфиг
		curr_power->battery_percent = percent;
		curr_power->battery_mvolt = mv;
		curr_power->is_charging = charging;

		// Выбираем иконку батареи в зависимости от процента
		const char* battery_symbols[5] = {
			LV_SYMBOL_BATTERY_EMPTY, LV_SYMBOL_BATTERY_1, LV_SYMBOL_BATTERY_2,
			LV_SYMBOL_BATTERY_3, LV_SYMBOL_BATTERY_FULL };
		int index = percent / 20;
		if ( index > 4 )
			index = 4;
		const char* battery_symbol = battery_symbols[index];

		// Заряд в процентах и иконка
		char temp_power_buf[16];
		if ( charging )
			snprintf( temp_power_buf, sizeof( temp_power_buf ), LV_SYMBOL_CHARGE " %s %d%%", battery_symbol, percent );
		else
			snprintf( temp_power_buf, sizeof( temp_power_buf ), "%s %d%%", battery_symbol, percent );

		// Обновляем subject
		lv_subject_copy_string( &subject_power_str, temp_power_buf );
		lv_subject_set_pointer( &subject_power, curr_power );
	}

	lvgl_port_unlock();
}

//-----------------------------------------------------------------------------------------
void project_config_set_wifi_state( pc_wifi_state_t state )
{
	lvgl_port_lock( 0 );

	if ( pc_config->wifi.state != state )
	{
		pc_config->wifi.state = state;

		// Обновляем UI (например, перерисовываем свитчер или скрываем список сетей)
		lv_subject_set_pointer( &subject_wifi, &pc_config->wifi );
	}

	lvgl_port_unlock();
}

//-----------------------------------------------------------------------------------------
void project_config_set_wifi_ap_list( const pc_ap_info* list, uint8_t count )
{
	lvgl_port_lock( 0 );

	// Защита от переполнения массива
	if ( count > 15 )
		count = 15;

	pc_wifi_config* curr_wifi = &pc_config->wifi;

	curr_wifi->ap_count = count;

	// Копируем список сетей (если он не пустой)
	if ( list != NULL && count > 0 )
	{
		for ( int i = 0; i < count; i++ )
		{
			curr_wifi->ap_list[i] = list[i];
		}
	}

	// Обновляем subject
	lv_subject_set_pointer( &subject_wifi, curr_wifi );

	lvgl_port_unlock();
}

//-----------------------------------------------------------------------------------------
void project_config_set_wifi_connected_ap( const pc_ap_info* ap, const char* ip )
{
	lvgl_port_lock( 0 );

	pc_wifi_config* curr_wifi = &pc_config->wifi;

	char temp_wifi_buf[8];

	if ( ap != NULL )
	{
		// 1. Выделяем память, если мы только что подключились
		if ( curr_wifi->connected_ap == NULL )
			curr_wifi->connected_ap = ( pc_ap_info* )malloc( sizeof( pc_ap_info ) );

		// 2. Копируем данные о сети
		*curr_wifi->connected_ap = *ap;

		// 3. Сохраняем IP-адрес
		if ( ip )
		{
			strncpy( curr_wifi->ip_address, ip, sizeof( curr_wifi->ip_address ) - 1 );
			curr_wifi->ip_address[sizeof( curr_wifi->ip_address ) - 1] = '\0';
		}

		// 4. Формируем иконку для циферблата
		if ( ap->rssi < -75 )
			strcpy( temp_wifi_buf, VW_SYMBOL_WIFI_LOW );
		else if ( ap->rssi < -60 )
			strcpy( temp_wifi_buf, VW_SYMBOL_WIFI_MID );
		else
			strcpy( temp_wifi_buf, LV_SYMBOL_WIFI );
	}
	else
	{
		// Отключились от сети: ОСВОБОЖДАЕМ ПАМЯТЬ
		if ( curr_wifi->connected_ap != NULL )
		{
			free( curr_wifi->connected_ap );
			curr_wifi->connected_ap = NULL;
		}

		// Очищаем сопутствующие данные
		memset( curr_wifi->ip_address, 0, sizeof( curr_wifi->ip_address ) );
		strcpy( temp_wifi_buf, "" );
	}

	// Обновляем subject
	lv_subject_copy_string( &subject_wifi_str, temp_wifi_buf );
	lv_subject_set_pointer( &subject_wifi, curr_wifi );

	lvgl_port_unlock();
}

//-----------------------------------------------------------------------------------------
void project_config_set_wifi_password( const char* pwd )
{
	lvgl_port_lock( 0 );
	if ( pwd )
	{
		strncpy( pc_config->wifi.password, pwd, sizeof( pc_config->wifi.password ) - 1 );
		pc_config->wifi.password[sizeof( pc_config->wifi.password ) - 1] = '\0';
	}
	lvgl_port_unlock();
}

//-----------------------------------------------------------------------------------------
void project_config_set_weather( int8_t temp, const char* icon, const char* desc )
{
	lvgl_port_lock( 0 );

	pc_config->weather.is_valid = true;
	pc_config->weather.temp = temp;

	if ( icon )
	{
		strncpy( pc_config->weather.icon, icon, sizeof( pc_config->weather.icon ) - 1 );
		pc_config->weather.icon[sizeof( pc_config->weather.icon ) - 1] = '\0';
	}
	if ( desc )
	{
		strncpy( pc_config->weather.description, desc, sizeof( pc_config->weather.description ) - 1 );
		pc_config->weather.description[sizeof( pc_config->weather.description ) - 1] = '\0';
	}

	lv_subject_set_pointer( &subject_weather, &pc_config->weather );

	lvgl_port_unlock();
}

//-----------------------------------------------------------------------------------------
// Геттеры
void project_config_get_power( pc_power_config* out_power )
{
	if ( !out_power )
		return;
	lvgl_port_lock( 0 );
	*out_power = pc_config->power; // Копируем структуру целиком
	lvgl_port_unlock();
}

//-----------------------------------------------------------------------------------------
void project_config_get_time( pc_time_config* out_time )
{
	if ( !out_time )
		return;
	lvgl_port_lock( 0 );
	*out_time = pc_config->time;
	lvgl_port_unlock();
}

//-----------------------------------------------------------------------------------------
void project_config_get_wifi( pc_wifi_config* out_wifi )
{
	if ( !out_wifi )
		return;
	lvgl_port_lock( 0 );
	*out_wifi = pc_config->wifi;
	lvgl_port_unlock();
}

//-----------------------------------------------------------------------------------------
void project_config_get_weather( pc_weather_config* out_weather )
{
	if ( !out_weather )
		return;
	lvgl_port_lock( 0 );
	*out_weather = pc_config->weather;
	lvgl_port_unlock();
}

//-----------------------------------------------------------------------------------------