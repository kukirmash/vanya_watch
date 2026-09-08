#ifndef BOARD_WIFI_H
#define BOARD_WIFI_H

#include <stdint.h>
#include <stdbool.h>

#define WIFI_MAX_SSID_LEN 32
#define WIFI_MAX_PASS_LEN 64

//-----------------------------------------------------------------------------------------
typedef struct 
{
    char ssid[WIFI_MAX_SSID_LEN];
    char password[WIFI_MAX_PASS_LEN];
    bool is_connected;
	
} wifi_config_t;

//-----------------------------------------------------------------------------------------
wifi_config_t* board_wifi_get_config(void);

//-----------------------------------------------------------------------------------------
void board_wifi_init(void);

//-----------------------------------------------------------------------------------------

#endif // BOARD_WIFI_H