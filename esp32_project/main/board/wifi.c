#include "wifi.h"

#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_netif_sntp.h"
#include "esp_sntp.h"

#include <string.h>

#include "config/project_config.h"

static const char* TAG = "WIFI";

#define MAX_RETRY 5 // Лимит попыток подключения
static int s_retry_num = 0;
static bool s_allow_reconnect = false;// Флаг, разрешающий переподключение 

//-----------------------------------------------------------------------------------------
// Внутренний обработчик событий Wi-Fi (асинхронная работа)
static void wifi_event_handler( void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data )
{
	// Wi-Fi запустился
	if ( event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START )
	{
		ESP_LOGI( TAG, "Wi-Fi successfully started." );
	}
	// Пропало соединение с точкей доступа Wi-Fi(или сами откючились)
	else if ( event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED )
	{
		ESP_LOGW( TAG, "Disconnected from AP." );
		project_config_set_wifi_status( false, "", "" );

		// Если соединение пропало само(а не выключили сами или еще на подключились)
		if ( s_allow_reconnect )
		{
			if ( s_retry_num < MAX_RETRY )
			{
				esp_wifi_connect();
				s_retry_num++;

				ESP_LOGI( TAG, "Retrying to connect... (%d/%d)", s_retry_num, MAX_RETRY );
			}
			else
			{
				ESP_LOGW( TAG, "Max retries reached. Stopping reconnects." );

				s_allow_reconnect = false; // Отключаем авто-повтор
			}
		}
	}
	// Успешно подлкючились к точке доступа Wi-Fi и получили ip адрес от dhcp
	else if ( event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP )
	{
		s_retry_num = 0;
		s_allow_reconnect = true; // Разрешаем переподключение в будущем при обрывах

		ip_event_got_ip_t* event = ( ip_event_got_ip_t* )event_data;
		char ip_str[16];
		snprintf( ip_str, sizeof( ip_str ), IPSTR, IP2STR( &event->ip_info.ip ) );

		ESP_LOGI( TAG, "Successfully connected to AP - IP: %s", ip_str );

		wifi_config_t conf;
		esp_wifi_get_config( WIFI_IF_STA, &conf );
		project_config_set_wifi_status( true, ( char* )conf.sta.ssid, ip_str );

		wifi_sntp_sync_time();// сразу синхронизируем время
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
}

//-----------------------------------------------------------------------------------------
int wifi_get_ap_info( int ap_count, wifi_ap_record_t* ap_info )
{
	if ( ap_info == NULL || ap_count <= 0 )
		return 0;

	ESP_ERROR_CHECK( esp_wifi_set_mode( WIFI_MODE_STA ) );

	pc_wifi_config pc_wifi_cfg;
	project_config_get_wifi( &pc_wifi_cfg );

	if ( !pc_wifi_cfg.is_enabled )
		return 0;

	ESP_LOGI( TAG, "Starting Wi-Fi scan..." );

	wifi_scan_config_t scan_config = { 0 };
	esp_err_t scan_err = esp_wifi_scan_start( &scan_config, true );

	// Wi-Fi сейчас подключается "STA is connecting" (ошибка 12294)
	if ( scan_err == ESP_ERR_WIFI_STATE )
	{
		ESP_LOGW( TAG, "Wi-Fi is busy connecting. Forcing disconnect to allow scan..." );

		s_allow_reconnect = false;
		esp_wifi_disconnect();
		vTaskDelay( pdMS_TO_TICKS( 100 ) );

		scan_err = esp_wifi_scan_start( &scan_config, true );
	}

	// Ошибка сканирования
	if ( scan_err != ESP_OK )
	{
		ESP_LOGE( TAG, "Scan failed: %d", scan_err );
		return 0;
	}

	uint16_t number_to_get = ap_count;
	uint16_t ap_found = 0;

	esp_wifi_scan_get_ap_num( &ap_found );
	esp_wifi_scan_get_ap_records( &number_to_get, ap_info );

	ESP_LOGI( TAG, "Total APs found: %u, returned to array: %u", ap_found, number_to_get );

	return ( int )number_to_get;
}

//-----------------------------------------------------------------------------------------
void wifi_connect_to_ap( const char* ssid, const char* password )
{
	pc_wifi_config pc_wifi_cfg;
	project_config_get_wifi( &pc_wifi_cfg );

	if ( !pc_wifi_cfg.is_enabled )
		return;

	s_retry_num = 0;
	s_allow_reconnect = true;

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

	esp_wifi_disconnect();
	ESP_ERROR_CHECK( esp_wifi_connect() );
}

//-----------------------------------------------------------------------------------------
// Отдельная задача для синхронизации времени
static void wifi_sntp_task( void* pvParameter )
{
	pc_wifi_config pc_wifi_cfg;
	project_config_get_wifi( &pc_wifi_cfg );

	if ( !pc_wifi_cfg.is_enabled )
	{
		vTaskDelete( NULL );
		return;
	}

	ESP_LOGI( TAG, "Initializing SNTP..." );

	if ( !esp_sntp_enabled() )
	{
		esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG( "pool.ntp.org" );
		esp_netif_sntp_init( &config );
	}

	ESP_LOGI( TAG, "Waiting for system time to be set..." );

	// Блокировка происходит ТОЛЬКО внутри этой задачи
	esp_err_t err = esp_netif_sntp_sync_wait( pdMS_TO_TICKS( 15000 ) );

	if ( err == ESP_OK )
	{
		ESP_LOGI( TAG, "Time synchronized successfully!" );
		setenv( "TZ", "MSK-3", 1 );
		tzset();
	}
	else
	{
		ESP_LOGE( TAG, "Failed to get time from NTP!" );
	}

	// Задача выполнена - уничтожаем её, чтобы освободить память
	vTaskDelete( NULL );
}

//-----------------------------------------------------------------------------------------
void wifi_sntp_sync_time( void )
{
	// Вместо блокировки потока, просто запускаем одноразовую задачу
	xTaskCreate( wifi_sntp_task, "sntp_task", 3072, NULL, 5, NULL );
}

//-----------------------------------------------------------------------------------------
void wifi_set_state( bool enable )
{
	if ( enable )
	{
		ESP_LOGI( TAG, "Turning Wi-Fi ON" );

		esp_wifi_start();
		project_config_set_wifi_enabled( true );
	}
	else
	{
		ESP_LOGI( TAG, "Turning Wi-Fi OFF" );

		s_retry_num = MAX_RETRY; // Отменяем любые попытки
		s_allow_reconnect = false;

		esp_wifi_disconnect();
		esp_wifi_stop();
		project_config_set_wifi_enabled( false );
		project_config_set_wifi_status( false, "", "" );
	}
}

//-----------------------------------------------------------------------------------------