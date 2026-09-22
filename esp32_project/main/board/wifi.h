#ifndef BOARD_WIFI_H
#define BOARD_WIFI_H

#include <stdint.h>
#include <stdbool.h>

#define WIFI_MAX_SSID_LEN 32
#define WIFI_MAX_PASS_LEN 64
#define WIFI_MAX_AP_COUNT 15

//-----------------------------------------------------------------------------------------
// Информация об одной найденной точке доступа
typedef struct
{
	char ssid[WIFI_MAX_SSID_LEN];
	int8_t rssi;
	bool is_secure;

} board_ap_info_t;

//-----------------------------------------------------------------------------------------
// Конфигурация Wi-Fi модуля (Singleton)
typedef struct
{
	char ssid[WIFI_MAX_SSID_LEN];
	char password[WIFI_MAX_PASS_LEN];
	bool is_connected;

	// Результаты последнего сканирования
	board_ap_info_t ap_list[WIFI_MAX_AP_COUNT];
	uint8_t ap_count;

} board_wifi_config_t;

//-----------------------------------------------------------------------------------------
// Возвращает указатель на Singleton конфигурацию Wi-Fi
board_wifi_config_t* board_wifi_get_config(void);

//-----------------------------------------------------------------------------------------
// Инициализация сетевого стека (NVS, esp_netif, esp_wifi) и запуск фоновой задачи
void board_wifi_init(void);

//-----------------------------------------------------------------------------------------
// Подключение к конкретной точке доступа (ssid/password)
void board_wifi_connect(const char* ssid, const char* password);

//-----------------------------------------------------------------------------------------

#endif // BOARD_WIFI_H
