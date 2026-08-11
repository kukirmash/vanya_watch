#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <lvgl/lvgl.h>

#include "lib/driver_backends.h"
#include "lib/simulator_settings.h"


extern simulator_settings_t settings;

int main(void)
{
    // 1. Инициализация LVGL
    lv_init();

    // 2. Настройка размеров окна эмулятора (Замените на VW_LCD_H_RES)
    settings.window_width = 240; 
    settings.window_height = 280;

    // 3. Запуск SDL бэкенда
    driver_backends_register();
    if(driver_backends_init_backend("SDL") == -1) {
        printf("Failed to initialize SDL backend\n");
        return -1;
    }

    // 4. Основное 


    // 5. Бесконечный цикл обработки событий LVGL
    while(1) {
        uint32_t ms = lv_timer_handler();
        if(ms == LV_NO_TIMER_READY) {
            ms = 5;
        }
        usleep(ms * 1000);
    }

    return 0;
}