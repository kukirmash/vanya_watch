#include "board/wifi.h"
#include "helpers/thread/thread_helper.h"
#include <stddef.h>

//-----------------------------------------------------------------------------------------
// Статический экземпляр конфигурации (Singleton)
static wifi_config_t current_wifi_config = {0};

//-----------------------------------------------------------------------------------------
wifi_config_t* board_wifi_get_config(void)
{
    return &current_wifi_config;
}

//-----------------------------------------------------------------------------------------
static void wifi_thread_cb(void *arg)
{
    while (1) 
    {
        // Здесь будет логика подключения к Wi-Fi на основе current_wifi_config
        thread_sleep_ms(1000); 
    }
}

//-----------------------------------------------------------------------------------------
void board_wifi_init(void)
{
    thread_create(wifi_thread_cb, NULL, 4096, 5, "wifi_thread");
}

//-----------------------------------------------------------------------------------------