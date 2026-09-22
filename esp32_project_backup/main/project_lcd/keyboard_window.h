#ifndef KEYBOARD_WINDOW_H
#define KEYBOARD_WINDOW_H

#include "lvgl_lcd_conf.h"

#if MOD_LVGL_LCD && VW_WORK_MODE

//-----------------------------------------------------------------------------------------
// Типы вводимых данных (пункт 3)
typedef enum
{
	KB_INPUT_TEXT,      // Обычный текст
	KB_INPUT_PASSWORD,  // Пароль (скрыт звездочками, все символы)
	KB_INPUT_NUM_ONLY,  // Только цифры
	KB_INPUT_PIN5       // Ровно 5 цифр
} kb_input_type;

//-----------------------------------------------------------------------------------------
// Сигнатура колбэка, который вызовется при нажатии "Done"
typedef void ( *keyboard_done_cb )( const char* text );

//-----------------------------------------------------------------------------------------
void keyboard_window_create( const char* title, kb_input_type type, keyboard_done_cb done_cb );

//-----------------------------------------------------------------------------------------
#endif // MOD_LVGL_LCD && VW_WORK_MODE
#endif // KEYBOARD_WINDOW_H
