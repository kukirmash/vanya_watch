#include "board/wifi.h"

#include "config/modules_config.h"

#if ESP32

#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_netif_sntp.h"
#include "esp_sntp.h"
#include "esp_log.h"

static const char* TAG = "BOARD_WIFI";

#define WIFI_MAX_RETRY       5
#define WIFI_SCAN_INTERVAL_S 10

//-----------------------------------------------------------------------------------------
// Singleton конфигурации
static board_wifi_config_t s_cfg = { 0 };

// Последняя сеть, к которой мы пытались подключиться
static char s_try_ssid[WIFI_MAX_SSID_LEN] = { 0 };
static char s_try_pass[WIFI_MAX_PASS_LEN] = { 0 };

static volatile bool s_connecting = false;
static volatile bool s_sntp_pending = false;
static int s_retry_num = 0;

//-----------------------------------------------------------------------------------------
board_wifi_config_t* board_wifi_get_config(void)
{
	return &s_cfg;
}

//-----------------------------------------------------------------------------------------
void board_wifi_connect(const char* ssid, const char* password)
{
	if (ssid == NULL)
		return;

	strncpy(s_cfg.ssid, ssid, sizeof(s_cfg.ssid) - 1);
	s_cfg.ssid[sizeof(s_cfg.ssid) - 1] = '\0';

	strncpy(s_cfg.password, password ? password : "", sizeof(s_cfg.password) - 1);
	s_cfg.password[sizeof(s_cfg.password) - 1] = '\0';

	s_cfg.is_connected = false;
}

//-----------------------------------------------------------------------------------------
static void start_sntp(void)
{
	if (!esp_sntp_enabled())
	{
		esp_sntp_config_t sntp_cfg = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
		esp_netif_sntp_init(&sntp_cfg);
		esp_sntp_setservername(1, "time.google.com");
		esp_sntp_setservername(2, "time.windows.com");
	}
}

//-----------------------------------------------------------------------------------------
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
	{
		// Авто-подключение к последней сохранённой в NVS сети
		wifi_config_t conf;
		if (esp_wifi_get_config(WIFI_IF_STA, &conf) == ESP_OK && conf.sta.ssid[0] != '\0')
		{
			strncpy(s_cfg.ssid, (char*)conf.sta.ssid, sizeof(s_cfg.ssid) - 1);
			strncpy(s_cfg.password, (char*)conf.sta.password, sizeof(s_cfg.password) - 1);
			strncpy(s_try_ssid, (char*)conf.sta.ssid, sizeof(s_try_ssid) - 1);
			strncpy(s_try_pass, (char*)conf.sta.password, sizeof(s_try_pass) - 1);
			ESP_LOGI(TAG, "Auto-connecting to saved AP: %s", s_cfg.ssid);
			s_retry_num = 0;
			s_connecting = true;
			esp_wifi_connect();
		}
	}
	else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
	{
		s_cfg.is_connected = false;

		if (s_connecting && s_retry_num < WIFI_MAX_RETRY)
		{
			s_retry_num++;
			ESP_LOGW(TAG, "Disconnected, retry %d/%d", s_retry_num, WIFI_MAX_RETRY);
			esp_wifi_connect();
		}
		else
		{
			s_connecting = false;
		}
	}
	else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
	{
		ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
		ESP_LOGI(TAG, "Connected - IP: " IPSTR, IP2STR(&event->ip_info.ip));
		s_cfg.is_connected = true;
		s_connecting = false;
		s_retry_num = 0;
		s_sntp_pending = true;
	}
}

//-----------------------------------------------------------------------------------------
// Сканирование доступных сетей (блокирующее)
static void wifi_do_scan(void)
{
	wifi_scan_config_t scan_config = { 0 };
	if (esp_wifi_scan_start(&scan_config, true) != ESP_OK)
		return;

	uint16_t found = 0;
	esp_wifi_scan_get_ap_num(&found);
	if (found > WIFI_MAX_AP_COUNT)
		found = WIFI_MAX_AP_COUNT;

	wifi_ap_record_t records[WIFI_MAX_AP_COUNT];
	if (found > 0 && esp_wifi_scan_get_ap_records(&found, records) == ESP_OK)
	{
		for (int i = 0; i < found; i++)
		{
			strncpy(s_cfg.ap_list[i].ssid, (char*)records[i].ssid, WIFI_MAX_SSID_LEN - 1);
			s_cfg.ap_list[i].ssid[WIFI_MAX_SSID_LEN - 1] = '\0';
			s_cfg.ap_list[i].rssi = records[i].rssi;
			s_cfg.ap_list[i].is_secure = (records[i].authmode != WIFI_AUTH_OPEN);
		}
		s_cfg.ap_count = (uint8_t)found;
	}
	else
	{
		s_cfg.ap_count = 0;
	}
}

//-----------------------------------------------------------------------------------------
// Фоновая задача: подключение по запросу, SNTP и периодическое сканирование
static void wifi_task(void* arg)
{
	uint32_t scan_timer = 0;

	while (1)
	{
		// 1. Подключение к запрошенной сети
		if (!s_cfg.is_connected && !s_connecting && s_cfg.ssid[0] != '\0' &&
			(strcmp(s_cfg.ssid, s_try_ssid) != 0 || strcmp(s_cfg.password, s_try_pass) != 0))
		{
			strncpy(s_try_ssid, s_cfg.ssid, sizeof(s_try_ssid) - 1);
			s_try_ssid[sizeof(s_try_ssid) - 1] = '\0';
			strncpy(s_try_pass, s_cfg.password, sizeof(s_try_pass) - 1);
			s_try_pass[sizeof(s_try_pass) - 1] = '\0';

			wifi_config_t wifi_config = { 0 };
			strncpy((char*)wifi_config.sta.ssid, s_cfg.ssid, sizeof(wifi_config.sta.ssid));
			strncpy((char*)wifi_config.sta.password, s_cfg.password, sizeof(wifi_config.sta.password));
			wifi_config.sta.threshold.authmode = (s_cfg.password[0] != '\0') ? WIFI_AUTH_WPA2_PSK : WIFI_AUTH_OPEN;

			ESP_LOGI(TAG, "Connecting to AP: %s", s_cfg.ssid);
			esp_wifi_set_mode(WIFI_MODE_STA);
			esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
			esp_wifi_disconnect();
			vTaskDelay(pdMS_TO_TICKS(50));

			s_retry_num = 0;
			s_connecting = true;
			esp_wifi_connect();
		}

		// 2. Синхронизация времени (выполняется один раз после подключения)
		if (s_sntp_pending)
		{
			s_sntp_pending = false;
			start_sntp();
			if (esp_netif_sntp_sync_wait(pdMS_TO_TICKS(10000)) == ESP_OK)
			{
				setenv("TZ", "MSK-3", 1);
				tzset();
				ESP_LOGI(TAG, "Time synchronized via SNTP");
			}
			else
			{
				ESP_LOGW(TAG, "SNTP sync timeout");
			}
		}

		// 3. Периодическое сканирование сетей
		if (++scan_timer >= WIFI_SCAN_INTERVAL_S)
		{
			scan_timer = 0;
			if (!s_connecting)
				wifi_do_scan();
		}

		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

//-----------------------------------------------------------------------------------------
void board_wifi_init(void)
{
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	esp_netif_create_default_wifi_sta();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
	ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_start());

	xTaskCreate(wifi_task, "wifi_task", 6144, NULL, 5, NULL);
}

//-----------------------------------------------------------------------------------------
#else
//-----------------------------------------------------------------------------------------
// Заглушка для не-ESP32 платформ (симулятор)
static board_wifi_config_t s_cfg = { 0 };

board_wifi_config_t* board_wifi_get_config(void)
{
	return &s_cfg;
}

void board_wifi_connect(const char* ssid, const char* password)
{
	(void)ssid;
	(void)password;
}

void board_wifi_init(void)
{
}
#endif // ESP32
