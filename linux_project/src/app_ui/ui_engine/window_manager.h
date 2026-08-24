#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include "lvgl/lvgl.h"

//-----------------------------------------------------------------------------------------
// Перечисление всех окон в системе
typedef enum 
{
    WIN_ID_NONE = 0,

    WIN_ID_WATCHFACE,
    WIN_ID_MENU,
    WIN_ID_WIFI,
    WIN_ID_SETTINGS,

    WIN_ID_COUNT
	
} window_id_t;

//-----------------------------------------------------------------------------------------
// Типы анимаций перехода
typedef enum 
{
    WIN_ANIM_NONE = 0,

    WIN_ANIM_SLIDE_LEFT,
    WIN_ANIM_SLIDE_RIGHT,
    WIN_ANIM_SLIDE_UP,
    WIN_ANIM_SLIDE_DOWN,
    WIN_ANIM_FADE,         // Плавное проявление

	WIN_ANIM_COUNT

} window_anim_t;

//-----------------------------------------------------------------------------------------
typedef lv_obj_t* (*wnd_create_cb)(void);

// Дескриптор окна
typedef struct 
{
    window_id_t id;
    wnd_create_cb create_cb;

} window_desc_t;

//-----------------------------------------------------------------------------------------
// Публичный API движка

// Инициализация менеджера
void window_manager_init(void);

// Регистрация окна в системе
void window_manager_register_wnd(window_id_t id, wnd_create_cb cb);

// Открыть окно с анимацией (добавляет окно в стек)
void window_open(window_id_t id, window_anim_t anim);

// Вернуться на предыдущее окно (свайп вправо / кнопка "Назад")
bool window_back(window_anim_t anim);

// Вернуться на циферблат (очищает весь стек до базы)
void return_to_watchface(void);

// Получить текущее активное окно
window_id_t current_window_get_id(void);

//-----------------------------------------------------------------------------------------
#endif // WINDOW_MANAGER_H