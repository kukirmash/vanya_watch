#include "wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_netif_sntp.h"
#include "esp_sntp.h"
#include <string.h>

static const char *TAG = "WIFI";

//-----------------------------------------------------------------------------------------
// Внутренний обработчик событий Wi-Fi (асинхронная работа)
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        // Wi-Fi модуль запущен, командуем подключиться
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        ESP_LOGW(TAG, "Disconnected from AP. Reconnecting...");
        // Если отключились, пытаемся переподключиться
        esp_wifi_connect();
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
    }
}

//-----------------------------------------------------------------------------------------
void wifi_init(void)
{
    // 1. Инициализация NVS (энергонезависимая память нужна для хранения калибровок Wi-Fi)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 2. Инициализация базового сетевого интерфейса
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    // 3. Базовая настройка радиомодуля
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // 4. Регистрация обработчика событий (чтобы ловить получение IP и отключения)
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));
}

//-----------------------------------------------------------------------------------------
int wifi_get_ap_info(int ap_count, wifi_ap_record_t *ap_info)
{
    if (ap_info == NULL || ap_count <= 0)
        return 0;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    // Запускаем Wi-Fi. Если он уже запущен ранее (вернул ESP_ERR_WIFI_STATE), игнорируем ошибку.
    esp_err_t err = esp_wifi_start();
    if (err != ESP_OK && err != ESP_ERR_WIFI_STATE)
    {
        ESP_ERROR_CHECK(err);
    }

    ESP_LOGI(TAG, "Starting Wi-Fi scan...");

    // Блокирующее сканирование (true - ждем завершения сканирования)
    esp_wifi_scan_start(NULL, true);

    uint16_t number_to_get = ap_count;
    uint16_t ap_found = 0;

    // Получаем общее количество найденных сетей
    esp_wifi_scan_get_ap_num(&ap_found);

    // Записываем данные в массив пользователя (но не больше, чем он просил)
    esp_wifi_scan_get_ap_records(&number_to_get, ap_info);

    ESP_LOGI(TAG, "Total APs found: %u, returned to array: %u", ap_found, number_to_get);

    return (int)number_to_get;
}

//-----------------------------------------------------------------------------------------
void wifi_connect_to_ap(const char *ssid, const char *password)
{
    wifi_config_t wifi_config = {0};

    // Копируем имя сети и пароль в структуру настроек
    strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
    strncpy((char *)wifi_config.sta.password, password, sizeof(wifi_config.sta.password));

    // Если пароль пустой, сеть открытая
    if (strlen(password) == 0)
    {
        wifi_config.sta.threshold.authmode = WIFI_AUTH_OPEN;
    }
    else
    {
        wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK; // Стандартная защита
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    ESP_LOGI(TAG, "Connecting to AP: %s", ssid);

    // Запускаем Wi-Fi
    esp_err_t err = esp_wifi_start();

    // Если модуль уже был запущен (например, после сканирования),
    // esp_wifi_start выдаст ESP_ERR_WIFI_STATE. В этом случае нужно просто послать команду connect.
    if (err == ESP_ERR_WIFI_STATE)
    {
        esp_wifi_connect();
    }
    else
    {
        ESP_ERROR_CHECK(err);
    }
}

//-----------------------------------------------------------------------------------------
// Отдельная задача для синхронизации времени
static void wifi_sntp_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Initializing SNTP...");
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
    esp_netif_sntp_init(&config);

    ESP_LOGI(TAG, "Waiting for system time to be set...");

    // Блокировка происходит ТОЛЬКО внутри этой задачи
    esp_err_t err = esp_netif_sntp_sync_wait(pdMS_TO_TICKS(15000));

    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "Time synchronized successfully!");
        setenv("TZ", "MSK-3", 1);
        tzset();
    }
    else
    {
        ESP_LOGE(TAG, "Failed to get time from NTP!");
    }

    // Задача выполнена - уничтожаем её, чтобы освободить память
    vTaskDelete(NULL);
}

//-----------------------------------------------------------------------------------------
void wifi_sntp_sync_time(void)
{
    // Вместо блокировки потока, просто запускаем одноразовую задачу
    xTaskCreate(wifi_sntp_task, "sntp_task", 3072, NULL, 5, NULL);
}

//-----------------------------------------------------------------------------------------