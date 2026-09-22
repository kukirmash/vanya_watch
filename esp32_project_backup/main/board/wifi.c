#include "wifi.h"

#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_netif_sntp.h"
#include "esp_sntp.h"

#include <string.h>

#include "config/project_config.h"
#include "weather.h"

static const char* TAG = "WIFI";

#define MAX_RETRY 5
static int s_retry_num = 0;

//-----------------------------------------------------------------------------------------
// Асинхронный обработчик событий от ядра ESP-IDF
static void wifi_event_handler( void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data )
{
	if ( event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START )
	{
		ESP_LOGI( TAG, "Wi-Fi started." );

		wifi_config_t conf;
		if ( esp_wifi_get_config( WIFI_IF_STA, &conf ) == ESP_OK && conf.sta.ssid[0] != '\0' )
		{
			ESP_LOGI( TAG, "Auto-connecting to saved AP: %s", conf.sta.ssid );
			project_config_set_wifi_state( WIFI_STATE_CONNECTING );
			s_retry_num = 0;
			esp_wifi_connect();
		}
		else
		{
			// Если сохраненной сети нет - просто переходим в режим поиска
			project_config_set_wifi_state( WIFI_STATE_DISCONNECTED );
		}
	}
	else if ( event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED )
	{
		ESP_LOGW( TAG, "Disconnected from AP." );
		project_config_set_wifi_connected_ap( NULL, NULL );

		project_config_lock();
		project_config* pc = project_config_get();
		pc_wifi_state_t current_state = pc->wifi.state;
		project_config_unlock();

		// Если мы были в процессе подключения или отвалились от сети, пробуем переподключиться
		if ( current_state == WIFI_STATE_CONNECTING || current_state == WIFI_STATE_CONNECTED )
		{
			if ( s_retry_num < MAX_RETRY )
			{
				project_config_set_wifi_state( WIFI_STATE_CONNECTING );

				esp_wifi_connect();
				s_retry_num++;

				ESP_LOGI( TAG, "Retrying to connect... (%d/%d)", s_retry_num, MAX_RETRY );
			}
			else
			{
				ESP_LOGW( TAG, "Max retries reached. Going to sleep mode." );
				project_config_set_wifi_state( WIFI_STATE_DISCONNECTED );
			}
		}
	}
	else if ( event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP )
	{
		s_retry_num = 0; // Сбрасываем попытки

		ip_event_got_ip_t* event = ( ip_event_got_ip_t* )event_data;
		char ip_str[16];
		snprintf( ip_str, sizeof( ip_str ), IPSTR, IP2STR( &event->ip_info.ip ) );

		ESP_LOGI( TAG, "Successfully connected to AP - IP: %s", ip_str );

		// Собираем данные
		pc_ap_info ap = { 0 };
		wifi_ap_record_t raw_ap;
		if ( esp_wifi_sta_get_ap_info( &raw_ap ) == ESP_OK )
		{
			strncpy( ap.ssid, ( char* )raw_ap.ssid, sizeof( ap.ssid ) - 1 );
			ap.rssi = raw_ap.rssi;
			ap.is_secure = ( raw_ap.authmode != WIFI_AUTH_OPEN );
		}

		project_config_set_wifi_connected_ap( &ap, ip_str );
		project_config_set_wifi_state( WIFI_STATE_CONNECTED );
	}
}

//-----------------------------------------------------------------------------------------
// Функция для выполнения сканирования
static bool execute_wifi_scan( void )
{
	wifi_scan_config_t scan_config = { 0 };
	if ( esp_wifi_scan_start( &scan_config, true ) == ESP_OK )
	{
		uint16_t ap_found = 0;
		esp_wifi_scan_get_ap_num( &ap_found );

		uint16_t number_to_get = ( ap_found > 15 ) ? 15 : ap_found;
		wifi_ap_record_t ap_info[15];
		esp_wifi_scan_get_ap_records( &number_to_get, ap_info );

		pc_ap_info safe_list[15];
		for ( int i = 0; i < number_to_get; i++ )
		{
			strncpy( safe_list[i].ssid, ( char* )ap_info[i].ssid, 32 );
			safe_list[i].ssid[32] = '\0';
			safe_list[i].rssi = ap_info[i].rssi;
			safe_list[i].is_secure = ( ap_info[i].authmode != WIFI_AUTH_OPEN );
		}
		project_config_set_wifi_ap_list( safe_list, number_to_get );

		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------------------
static bool execute_sntp_sync( void )
{
	ESP_LOGI( TAG, "Manager: Starting SNTP sync..." );

	if ( !esp_sntp_enabled() )
	{
		esp_sntp_config_t sntp_cfg = ESP_NETIF_SNTP_DEFAULT_CONFIG( "pool.ntp.org" );
		esp_netif_sntp_init( &sntp_cfg );
		esp_sntp_setservername( 1, "time.google.com" );
		esp_sntp_setservername( 2, "time.windows.com" );
	}

	for ( int i = 0; i < 5; i++ )
	{
		if ( esp_netif_sntp_sync_wait( pdMS_TO_TICKS( 2000 ) ) == ESP_OK )
		{
			ESP_LOGI( TAG, "Time synchronized successfully!" );
			setenv( "TZ", "MSK-3", 1 );
			tzset();
			return true;
		}
	}

	esp_netif_sntp_deinit();
	ESP_LOGW( TAG, "Time was not synchronized" );
	return false;
}

//-----------------------------------------------------------------------------------------
static void wifi_task( void* arg )
{
	// TODO: константы для таймеров вынести в конфиг
	uint32_t scan_timer = 15;
	uint32_t sntp_timer = 86400; // 24 часа
	uint32_t weather_timer = 3600; // 1 час

	while ( 1 )
	{
		project_config_lock();
		project_config* pc = project_config_get();
		pc_wifi_state_t state = pc->wifi.state;
		project_config_unlock();

		scan_timer++;
		sntp_timer++;
		weather_timer++;

		switch ( state )
		{
		case WIFI_STATE_DISABLED:
		case WIFI_STATE_CONNECTING:
		case WIFI_STATE_BUSY:
			// Блокировка: ничего не делаем
			break;

		case WIFI_STATE_DISCONNECTED:
		{
			if ( scan_timer >= 10 )
			{
				project_config_set_wifi_state( WIFI_STATE_BUSY );
				bool res = execute_wifi_scan(); // БЛОКИРУЕТ ПОТОК
				if ( res )
					scan_timer = 0;

				project_config_lock();
				if ( project_config_get()->wifi.state == WIFI_STATE_BUSY )
					project_config_set_wifi_state( WIFI_STATE_DISCONNECTED );
				project_config_unlock();
			}
			break;
		}
		case WIFI_STATE_CONNECTED:
		{
			// ПРИОРИТЕТ 1: SNTP
			if ( sntp_timer >= 86400 )
			{
				project_config_set_wifi_state( WIFI_STATE_BUSY );
				bool res = execute_sntp_sync(); // БЛОКИРУЕТ ПОТОК
				if ( res )
					sntp_timer = 0;

				project_config_lock();
				if ( project_config_get()->wifi.state == WIFI_STATE_BUSY )
					project_config_set_wifi_state( WIFI_STATE_CONNECTED );
				project_config_unlock();
			}
			// ПРИОРИТЕТ 2: Погода
			else if ( weather_timer >= 3600 )
			{
				project_config_set_wifi_state( WIFI_STATE_BUSY );
				bool res = weather_update_sync(); // БЛОКИРУЕТ ПОТОК
				if ( res )
					weather_timer = 0;

				project_config_lock();
				if ( project_config_get()->wifi.state == WIFI_STATE_BUSY )
					project_config_set_wifi_state( WIFI_STATE_CONNECTED );
				project_config_unlock();
			}
			// ПРИОРИТЕТ 3: Сканирование сетей
			else if ( scan_timer >= 15 )
			{
				project_config_set_wifi_state( WIFI_STATE_BUSY );
				bool res = execute_wifi_scan(); // БЛОКИРУЕТ ПОТОК
				if ( res )
					scan_timer = 0;

				project_config_lock();
				if ( project_config_get()->wifi.state == WIFI_STATE_BUSY )
					project_config_set_wifi_state( WIFI_STATE_CONNECTED );
				project_config_unlock();
			}
			break;
		}
		}

		vTaskDelay( pdMS_TO_TICKS( 1000 ) );
	}
}

//-----------------------------------------------------------------------------------------
void wifi_set_state( bool enable )
{
	nvs_handle_t my_handle;
	if ( nvs_open( "storage", NVS_READWRITE, &my_handle ) == ESP_OK )
	{
		nvs_set_u8( my_handle, "wifi_on", enable ? 1 : 0 );
		nvs_commit( my_handle );
		nvs_close( my_handle );
	}

	if ( enable )
	{
		ESP_LOGI( TAG, "Turning Wi-Fi ON" );
		esp_wifi_start(); // Сгенерирует событие STA_START и переведет в DISCONNECTED
	}
	else
	{
		ESP_LOGI( TAG, "Turning Wi-Fi OFF" );
		project_config_set_wifi_state( WIFI_STATE_DISABLED ); // Сразу глушим автомат

		esp_wifi_disconnect();
		esp_wifi_stop();

		project_config_set_wifi_connected_ap( NULL, NULL );
		project_config_set_wifi_ap_list( NULL, 0 );
	}
}

//-----------------------------------------------------------------------------------------
void wifi_init( void )
{
	esp_err_t ret = nvs_flash_init();
	if ( ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND )
	{
		ESP_ERROR_CHECK( nvs_flash_erase() );
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK( ret );

	ESP_ERROR_CHECK( esp_netif_init() );
	ESP_ERROR_CHECK( esp_event_loop_create_default() );
	esp_netif_create_default_wifi_sta();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK( esp_wifi_init( &cfg ) );

	ESP_ERROR_CHECK( esp_event_handler_instance_register( WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL ) );
	ESP_ERROR_CHECK( esp_event_handler_instance_register( IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL ) );

	xTaskCreate( wifi_task, "wifi_manager", 6144, NULL, 5, NULL );

	uint8_t wifi_was_on = 0;
	nvs_handle_t my_handle;
	if ( nvs_open( "storage", NVS_READONLY, &my_handle ) == ESP_OK )
	{
		nvs_get_u8( my_handle, "wifi_on", &wifi_was_on );
		nvs_close( my_handle );
	}

	if ( wifi_was_on )
		wifi_set_state( true );
}

//-----------------------------------------------------------------------------------------
void wifi_connect_to_ap( const char* ssid, const char* password )
{
	project_config_lock();
	pc_wifi_state_t state = project_config_get()->wifi.state;
	project_config_unlock();

	if ( state == WIFI_STATE_DISABLED )
		return;

	project_config_set_wifi_password( password );
	project_config_set_wifi_state( WIFI_STATE_CONNECTING ); // Блокируем сканер

	esp_wifi_disconnect();
	vTaskDelay( pdMS_TO_TICKS( 30 ) );

	wifi_config_t wifi_config = { 0 };
	strncpy( ( char* )wifi_config.sta.ssid, ssid, sizeof( wifi_config.sta.ssid ) );
	strncpy( ( char* )wifi_config.sta.password, password, sizeof( wifi_config.sta.password ) );

	if ( strlen( password ) == 0 )
		wifi_config.sta.threshold.authmode = WIFI_AUTH_OPEN;
	else
		wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

	ESP_ERROR_CHECK( esp_wifi_set_mode( WIFI_MODE_STA ) );
	ESP_ERROR_CHECK( esp_wifi_set_config( WIFI_IF_STA, &wifi_config ) );

	ESP_LOGI( TAG, "Connecting to AP: %s", ssid );

	s_retry_num = 0;
	ESP_ERROR_CHECK( esp_wifi_connect() );
}

//-----------------------------------------------------------------------------------------