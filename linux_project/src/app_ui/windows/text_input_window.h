#ifndef TEXT_INPUT_WINDOW_H
#define TEXT_INPUT_WINDOW_H

#include "config/ui_config.h"

//-----------------------------------------------------------------------------------------
// Функция для передачи контекста (заголовка и буфера) перед открытием окна
void text_input_set_context(const char* title, char* output_buffer, size_t buffer_size);

//-----------------------------------------------------------------------------------------
void text_input_window_init(lv_obj_t* parent);

//-----------------------------------------------------------------------------------------

#endif // TEXT_INPUT_WINDOW_H