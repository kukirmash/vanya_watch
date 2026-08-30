#include "app_ui/ui_engine/window_manager.h"

#include "config/ui_config.h"
#include <stdio.h>

//-----------------------------------------------------------------------------------------
typedef struct
{
    window_id_t id;
    wnd_init_cb init_cb;
    window_swipe_targets_t swipes;
} window_desc_t;

typedef struct
{
    window_id_t id;
    lv_obj_t *obj;
} window_stack_item_t;

//-----------------------------------------------------------------------------------------
static window_desc_t desc_windows[WIN_ID_COUNT];
static window_stack_item_t menu_windows[MAX_MENU_DEPTH];
static int menu_top = -1;

//-----------------------------------------------------------------------------------------
// Автоматически определяет тип анимации на основе направления свайпа
static window_anim_t get_anim_by_dir(lv_dir_t dir)
{
    if (dir == LV_DIR_LEFT)
        return WIN_ANIM_SLIDE_LEFT;
    if (dir == LV_DIR_RIGHT)
        return WIN_ANIM_SLIDE_RIGHT;
    if (dir == LV_DIR_TOP)
        return WIN_ANIM_SLIDE_UP;
    if (dir == LV_DIR_BOTTOM)
        return WIN_ANIM_SLIDE_DOWN;
    return WIN_ANIM_NONE;
}

//-----------------------------------------------------------------------------------------
// Универсальный обработчик жестов, выполняющий навигацию по маршрутам окна
static void global_gesture_event_cb(lv_event_t *e)
{
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
    window_id_t current_id = current_window_get_id();

    if (current_id == WIN_ID_NONE)
        return;

    window_id_t target_id = WIN_ID_NONE;

    // Извлечение целевого окна для конкретного жеста
    if (dir == LV_DIR_LEFT)
        target_id = desc_windows[current_id].swipes.left;
    else if (dir == LV_DIR_RIGHT)
        target_id = desc_windows[current_id].swipes.right;
    else if (dir == LV_DIR_TOP)
        target_id = desc_windows[current_id].swipes.up;
    else if (dir == LV_DIR_BOTTOM)
        target_id = desc_windows[current_id].swipes.down;

    // Если маршрут для свайпа не задан, ничего не делаем
    if (target_id == WIN_ID_NONE)
        return;

    window_anim_t anim = get_anim_by_dir(dir);

    // Умный возврат: если целевое окно совпадает с предыдущим в стеке, вызываем window_back
    if (menu_top > 0 && menu_windows[menu_top - 1].id == target_id)
        window_back(anim);
    else
        window_open(target_id, anim);
}

//-----------------------------------------------------------------------------------------
// Коллбэк вызывается ядром LVGL ровно в тот момент, когда анимация полностью завершилась
static void anim_ready_delete_cb(lv_anim_t *a)
{
    // Безопасно получаем объект окна из анимации и удаляем его
    lv_obj_t *obj = (lv_obj_t *)lv_anim_get_var(a);
    if (obj)
    {
        lv_obj_delete_async(obj);
    }
}

//-----------------------------------------------------------------------------------------
// Функция-обертка для анимации прозрачности, так как базовой функции нужны 3 аргумента
static void anim_set_opa_cb(void *var, int32_t v)
{
    lv_obj_set_style_opa((lv_obj_t *)var, v, LV_PART_MAIN);
}

//-----------------------------------------------------------------------------------------
// Применяет анимацию к объекту (настроено симметрично для корректной работы стека)
static void apply_transition_animation(lv_obj_t *obj, window_anim_t anim, bool is_appearing, bool delete_after)
{
    if (obj == NULL)
        return;

    // Если анимация отключена, но окно нужно удалить - удаляем его сразу
    if (anim == WIN_ANIM_NONE)
    {
        if (delete_after)
            lv_obj_delete_async(obj);

        return;
    }

    // 1. Форсируем пересчет размеров
    lv_obj_update_layout(obj);

    // ВАЖНО: Очистка "остаточных" состояний от предыдущих анимаций
    if (anim != WIN_ANIM_FADE) {
        // Если это слайд, гарантируем, что окно не осталось прозрачным от старого FADE
        lv_obj_set_style_opa(obj, LV_OPA_COVER, LV_PART_MAIN);
    }
    if (anim != WIN_ANIM_SLIDE_LEFT && anim != WIN_ANIM_SLIDE_RIGHT) {
        // Если это не горизонтальный слайд, обнуляем X
        lv_obj_set_x(obj, 0);
    }
    if (anim != WIN_ANIM_SLIDE_UP && anim != WIN_ANIM_SLIDE_DOWN) {
        // Если это не вертикальный слайд, обнуляем Y
        lv_obj_set_y(obj, 0);
    }

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_time(&a, 250);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);

    int32_t start_val = 0, end_val = 0;
    lv_anim_exec_xcb_t exec_cb = NULL;

    int32_t w = lv_obj_get_width(obj);
    int32_t h = lv_obj_get_height(obj);

    switch (anim)
    {
    case WIN_ANIM_SLIDE_LEFT:
        exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_x;
        start_val = is_appearing ? w : 0;
        end_val = is_appearing ? 0 : -w;
        break;

    case WIN_ANIM_SLIDE_RIGHT:
        exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_x;
        start_val = is_appearing ? -w : 0;
        end_val = is_appearing ? 0 : w;
        break;

    case WIN_ANIM_SLIDE_UP:
        exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_y;
        start_val = is_appearing ? h : 0;
        end_val = is_appearing ? 0 : -h;
        break;

    case WIN_ANIM_SLIDE_DOWN:
        exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_y;
        start_val = is_appearing ? -h : 0;
        end_val = is_appearing ? 0 : h;
        break;

    case WIN_ANIM_FADE:
        exec_cb = (lv_anim_exec_xcb_t)anim_set_opa_cb;
        start_val = is_appearing ? LV_OPA_TRANSP : LV_OPA_COVER;
        end_val = is_appearing ? LV_OPA_COVER : LV_OPA_TRANSP;
        break;

    default:
        return;
    }

    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_set_values(&a, start_val, end_val);

    // Если флаг установлен - вешаем коллбэк для удаления после окончания анимации
    if (delete_after) 
        lv_anim_set_ready_cb(&a, anim_ready_delete_cb);

    // 2. Устанавливаем окно в начальную позицию ДО старта анимации
    if (exec_cb)
    {
        exec_cb(obj, start_val);
    }

    lv_anim_start(&a);
}
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
void window_manager_init(void)
{
    menu_top = -1;

    lv_obj_add_event_cb(lv_screen_active(), global_gesture_event_cb, LV_EVENT_GESTURE, NULL);
}

//-----------------------------------------------------------------------------------------
void window_manager_register_wnd(window_id_t id, wnd_init_cb cb, window_swipe_targets_t swipes)
{
    if (id > WIN_ID_NONE && id < WIN_ID_COUNT)
    {
        desc_windows[id].id = id;
        desc_windows[id].init_cb = cb;
        desc_windows[id].swipes = swipes;
    }
}

//-----------------------------------------------------------------------------------------
void window_open(window_id_t id, window_anim_t anim)
{
    if (id <= WIN_ID_NONE || id >= WIN_ID_COUNT || desc_windows[id].init_cb == NULL || menu_top >= MAX_MENU_DEPTH - 1)
        return;

    if (menu_top >= 0)
        apply_transition_animation(menu_windows[menu_top].obj, anim, false, false);

    // Создаем холст для нового окна и настраиваем базовые стили (цвет, отсутствие рамок)
    lv_obj_t *new_win = lv_obj_create(lv_screen_active());
    lv_obj_set_size(new_win, lv_pct(100), lv_pct(100));
    lv_obj_set_scrollbar_mode(new_win, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_color(new_win, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_border_width(new_win, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(new_win, 0, LV_PART_MAIN);

    // Вызываем функцию инициализации самого окна, передавая созданный холст
    desc_windows[id].init_cb(new_win);

    menu_top++;
    menu_windows[menu_top].id = id;
    menu_windows[menu_top].obj = new_win;

    apply_transition_animation(new_win, anim, true, false);
}

//-----------------------------------------------------------------------------------------
bool window_back(window_anim_t anim)
{
    if (menu_top <= 0)
        return false;

    lv_obj_t *closing_win = menu_windows[menu_top].obj;
    apply_transition_animation(closing_win, anim, false, true); // удаление после анимации

    menu_top--;

    lv_obj_t *prev_win = menu_windows[menu_top].obj;
    apply_transition_animation(prev_win, anim, true, false);

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