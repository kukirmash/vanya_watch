#ifndef THREAD_HELPER_H
#define THREAD_HELPER_H

#include <stdint.h>

//-----------------------------------------------------------------------------------------
// Универсальная сигнатура функции потока
typedef void (*thread_cb_t)(void *arg);

//-----------------------------------------------------------------------------------------
void thread_create(thread_cb_t callback, void *arg, uint32_t stack_size, uint8_t priority, const char *name);

//-----------------------------------------------------------------------------------------
void thread_sleep_ms(uint32_t milliseconds);

//-----------------------------------------------------------------------------------------

#endif // THREAD_HELPER_H