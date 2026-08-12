#include "app_ui/ui_engine/window_manager.h"

#if MOD_LVGL_LCD && VW_WORK_MODE

#include <stdio.h>

//-----------------------------------------------------------------------------------------
// Стек открытых окон
typedef struct
{
    window_id_t id;
    lv_obj_t *obj;

} window_stack_item_t;

static window_desc_t desc_windows[WIN_ID_COUNT];
static window_stack_item_t menu_windows[MAX_MENU_DEPTH];
static int menu_top = -1;

//-----------------------------------------------------------------------------------------
// Глобальный обработчик свайпов для навигации
static void global_gesture_event_cb(lv_event_t *e)
{
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
    window_id_t current_id = current_window_get_id();

    // Свайп вправо -> Возврат назад (кроме главного циферблата)
    if (dir == LV_DIR_RIGHT && current_id != WIN_ID_WATCHFACE)
    {
        window_back(WIN_ANIM_SLIDE_RIGHT);
    }
    // Свайп влево на циферблате -> Открытие меню
    else if (dir == LV_DIR_LEFT && current_id == WIN_ID_WATCHFACE)
    {
        window_open(WIN_ID_MENU, WIN_ANIM_SLIDE_LEFT);
    }
}

//-----------------------------------------------------------------------------------------
// Анимация переходов между экранами
static void apply_transition_animation(lv_obj_t *obj, window_anim_t anim, bool is_appearing)
{
    if (anim == WIN_ANIM_NONE || obj == NULL)
        return;

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_time(&a, 250); // Время анимации 250 мс
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);

    int32_t start_val = 0, end_val = 0;
    lv_anim_exec_xcb_t exec_cb = NULL;

    int32_t w = lv_obj_get_width(obj);
    int32_t h = lv_obj_get_height(obj);

    switch (anim)
    {
    case WIN_ANIM_SLIDE_LEFT: // Новое окно выезжает справа
        exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_x;
        start_val = is_appearing ? w : 0;
        end_val = is_appearing ? 0 : -w / 3;
        break;

    case WIN_ANIM_SLIDE_RIGHT: // Окно уезжает вправо
        exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_x;
        start_val = is_appearing ? -w / 3 : 0;
        end_val = is_appearing ? 0 : w;
        break;

    case WIN_ANIM_SLIDE_UP: // Выезжает снизу
        exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_y;
        start_val = is_appearing ? h : 0;
        end_val = is_appearing ? 0 : -h;
        break;

    case WIN_ANIM_FADE: // Проявление
        exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_style_opa;
        start_val = is_appearing ? LV_OPA_TRANSP : LV_OPA_COVER;
        end_val = is_appearing ? LV_OPA_COVER : LV_OPA_TRANSP;
        break;

    default:
        return;
    }

    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_set_values(&a, start_val, end_val);
    lv_anim_start(&a);
}

//-----------------------------------------------------------------------------------------
void window_manager_init(void)
{
    menu_top = -1;
}

//-----------------------------------------------------------------------------------------
void window_manager_register_wnd(window_id_t id, wnd_create_cb cb)
{
    if (id > WIN_ID_NONE && id < WIN_ID_COUNT)
    {
        desc_windows[id].id = id;
        desc_windows[id].create_cb = cb;
    }
}

//-----------------------------------------------------------------------------------------
void window_open(window_id_t id, window_anim_t anim)
{
    if (id <= WIN_ID_NONE || id >= WIN_ID_COUNT || desc_windows[id].create_cb == NULL)
        return;

    // 1. Анимируем уход текущего верхнего окна (если есть)
    if (menu_top >= 0)
        apply_transition_animation(menu_windows[menu_top].obj, anim, false);

    // 2. Создаем новое окно
    lv_obj_t *new_win = desc_windows[id].create_cb();
    if (new_win == NULL)
        return;

    // Включаем обработку свайпов на новом окне
    lv_obj_add_event_cb(new_win, global_gesture_event_cb, LV_EVENT_GESTURE, NULL);

    // 3. Кладем в стек
    if (menu_top < MAX_MENU_DEPTH - 1)
    {
        menu_top++;
        menu_windows[menu_top].id = id;
        menu_windows[menu_top].obj = new_win;
    }

    // 4. Анимируем появление нового окна
    apply_transition_animation(new_win, anim, true);
}

//-----------------------------------------------------------------------------------------
bool window_back(window_anim_t anim)
{
    if (menu_top <= 0)
        return false; // Настоятельно не закрываем самый базовый экран (циферблат)

    // 1. Анимируем и удаляем текущее окно
    lv_obj_t *closing_win = menu_windows[menu_top].obj;
    apply_transition_animation(closing_win, anim, false);

    // Асинхронно удаляем объект окна из памяти через LVGL
    lv_obj_delete_async(closing_win);
    menu_top--;

    // 2. Возвращаем видимость предыдущему окну
    lv_obj_t *prev_win = menu_windows[menu_top].obj;
    apply_transition_animation(prev_win, anim, true);

    return true;
}

//-----------------------------------------------------------------------------------------
void return_to_watchface(void)
{
    while (menu_top > 0)
    {
        lv_obj_delete_async(menu_windows[menu_top].obj);
        menu_top--;
    }
}

//-----------------------------------------------------------------------------------------
window_id_t current_window_get_id(void)
{
    if (menu_top >= 0)
        return menu_windows[menu_top].id;
    return WIN_ID_NONE;
}

//-----------------------------------------------------------------------------------------

#endif // MOD_LVGL_LCD && VW_WORK_MODE