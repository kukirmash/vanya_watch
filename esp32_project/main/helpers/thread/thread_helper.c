#include "helpers/thread/thread_helper.h"

#include "config/modules_config.h"

#if ESP32
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
#elif LINUX
    #include <pthread.h>
    #include <unistd.h>
#endif

//-----------------------------------------------------------------------------------------
void thread_create(thread_cb_t callback, void *arg, uint32_t stack_size, uint8_t priority, const char *name)
{
#if ESP32
    xTaskCreate(callback, name, stack_size, arg, priority, NULL);
#elif LINUX
    pthread_t tid;
    // Для Linux приоритет и размер стека пока опускаем (можно добавить через pthread_attr_t, если потребуется)
    pthread_create(&tid, NULL, (void *(*)(void *))callback, arg);
    pthread_detach(tid); // Отвязываем поток, чтобы не было утечек памяти без вызова join
#endif
}

//-----------------------------------------------------------------------------------------
void thread_sleep_ms(uint32_t milliseconds)
{
#if ESP32
    vTaskDelay(pdMS_TO_TICKS(milliseconds));
#elif LINUX
    usleep(milliseconds * 1000);
#endif
}

//-----------------------------------------------------------------------------------------