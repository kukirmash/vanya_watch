#ifndef MODULE_CONFIG_H
#define MODULE_CONFIG_H

#define WINDOWS         1
#define ESP32           0

#define MOD_LVGL_LCD    1

//-----------------------------------------------------------------------------------------
// КРОССПЛАТФОРМЕННЫЕ МАКРОСЫ И ОБЕРТКИ
#if WINDOWS

#include <stdio.h>
#define LOGI(TAG, format, ...) printf("I (%lu) %s: " format "\n", (unsigned long)clock(), TAG, ##__VA_ARGS__)

#elif ESP32
#include "esp_log.h"

#define LOGI(TAG, format, ...) ESP_LOGI(TAG, format, ##__VA_ARGS__)

#endif
//-----------------------------------------------------------------------------------------

#endif // MODULE_CONFIG_H
