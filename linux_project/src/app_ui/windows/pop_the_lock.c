#include "app_ui/windows/pop_the_lock.h"

#include "stdio.h"
#include "math.h"
#include "app_ui/ui_engine/window_manager.h"
#include "config/lcd_config.h" // VW_LCD_V_RES, VW_LCD_H_RES

static const char* TAG = "VW_POP_THE_LOCK";

//-----------------------------------------------------------------------------------------
#define RADIUS  100
#define WIDTH   30

#define GOAL_SIZE       (WIDTH - 6)
#define CENTER_RADIUS   (RADIUS - (WIDTH / 2))
#define CENTER_X        (VW_LCD_H_RES / 2.0f)
#define CENTER_Y        (VW_LCD_V_RES / 2.0f)
#define POINTER_WIDTH   (8)

#define GAME_OVER_COLOR_HEX     0xE9685C
#define LOCK_COLOR_HEX          0x1F0431
#define LOCK_HANDLE_COLOR_HEX   0x314162
#define GOAL_COLOR_HEX          0xEFCC3C
#define POINTER_COLOR_HEX       0xE82C5E
#define SCORE_COLOR_HEX         0xAEE7EE
#define BACKGROUND_COLOR_HEX    0x5CD3D9

#define M_PI 3.14159265358979323846 // pi
#define radians(degrees) ((degrees) * M_PI / 180)
#define degrees(radians) ((radians) * 180 / M_PI)

//-----------------------------------------------------------------------------------------
// Состояние игры
typedef enum {
    GAME_READY = 0,                 // Ожидание нажатия
    GAME_POINTER_BEFORE_GOAL = 1,   // Указатель не достиг цели
    GAME_POINTER_INSIDE_GOAL = 2,   // Указатель достиг цели
    GAME_OVER = 3,                  // Игра проиграна

    GAME_STATE_CNT
} game_state_t;

//-----------------------------------------------------------------------------------------
// Направление движения указателя
typedef enum {
    CLOCKWISE = -1,       // По часовой стрелке
    COUNTERCLOCKWISE = 1  // Против часовой стрелки
} pointer_dir_t;

//-----------------------------------------------------------------------------------------
// Константные параметры указателя
// Начальное положение указателя
#define POINTER_INITIAL_POSITION radians(90)
// Начальное направление указателя
#define POINTER_INITAIL_DIRECTION COUNTERCLOCKWISE

// Константные параметры цели
// Минимальное расстояние между указателем и появившейся целью
#define GOAL_SPAWN_DISTANCE radians(30)
// Длина дуги, на которой появляется цель
#define GOAL_SPAWN_LENGTH radians(180)
// Длина дуги, на которой будет засчитано попадание указателя в цель
#define GOAL_LENGTH  radians(10)

//-----------------------------------------------------------------------------------------
// Текущий уровень
static int level;
// Текущий счёт
static int score;
// Положение указателя в радианах
static float pointer_position = POINTER_INITIAL_POSITION;
// Направление указателя
static pointer_dir_t pointer_direction;
// Скорость указателя (рад/с)
static float pointer_speed;
// Состояние игры
static game_state_t game_state;
// Позиция цели в радианах
static float goal_position;

static lv_obj_t* scale = NULL;
static lv_obj_t* pointer = NULL;
static lv_timer_t* game_timer = NULL;

//-----------------------------------------------------------------------------------------
// Устанавливает позицию цели в радианах
static void goal_obj_set_pos_rad(lv_obj_t* goal_obj, float positon_rad)
{
    if (goal_obj == NULL)
        return;

    int32_t goal_x = RADIUS + CENTER_RADIUS * cosf(positon_rad) - GOAL_SIZE / 2;
    int32_t goal_y = RADIUS - CENTER_RADIUS * sinf(positon_rad) - GOAL_SIZE / 2;

    lv_obj_set_pos(goal_obj, goal_x, goal_y);
}

//-----------------------------------------------------------------------------------------
// Устанавливает позицию указателя в радианах
static void pointer_set_pos_rad(lv_obj_t* scale, lv_obj_t* pointer, float position_rad)
{
    if (scale == NULL || pointer == NULL)
        return;

    // Перевод радиан в градусы, для lv_scale
    int32_t deg = 360 - ((int32_t)(degrees(position_rad)) % 360 + 360) % 360;
    lv_scale_set_line_needle_value(scale, pointer, RADIUS, deg);
}

//-----------------------------------------------------------------------------------------
static void update(lv_timer_t* timer)
{
    const float dt = 0.033f;

    switch (game_state) {
        // Если активное состояние игры
    case GAME_POINTER_BEFORE_GOAL:
    case GAME_POINTER_INSIDE_GOAL:
        // Обновить положение игрока
        pointer_position += dt * pointer_direction * pointer_speed;
        pointer_set_pos_rad(scale, pointer, pointer_position);

        break;
    }
}

//-----------------------------------------------------------------------------------------
// Инициализирует таймер
// если не создан - создает
// если на паузе - возобновляет
// 33 = 1000 мс / 30 FPS
static void init_timer()
{
    if (game_timer == NULL)
        game_timer = lv_timer_create(update, 33, NULL);
    else
    {
        bool is_paused = lv_timer_get_paused(game_timer);
        if (is_paused)
            lv_timer_resume(game_timer);
        else
        {

        }
    }
}

//-----------------------------------------------------------------------------------------
// Функция перезапуска игры
static void restart()
{
    // Начальный счёт равняется уровню
    score = level;

    // Установка начального положения указателя
    pointer_position = POINTER_INITIAL_POSITION;
    pointer_set_pos_rad(scale, pointer, pointer_position);

    // Установка начального направления указателя
    pointer_direction = POINTER_INITAIL_DIRECTION;
    // Установка скорости TODO переделать 
    pointer_speed = radians(75 + level);

    // Установка начального состояния игры
    game_state = GAME_POINTER_BEFORE_GOAL;

    init_timer();
}
//-----------------------------------------------------------------------------------------
// Функция инициализации игры
static void init()
{
    // Игра начинается с 1 уровня
    level = 1;
    restart();
}
//-----------------------------------------------------------------------------------------
static void content_click_cb(lv_event_t* e)
{
    init();
}

//-----------------------------------------------------------------------------------------
// Создает экран игры "Pop The Lock"
void pop_the_lock_init(lv_obj_t* parent)
{
    lv_obj_t* content = parent;
    lv_obj_set_style_bg_color(content, lv_color_hex(BACKGROUND_COLOR_HEX), LV_PART_MAIN);
    lv_obj_add_event_cb(content, content_click_cb, LV_EVENT_CLICKED, NULL);

    scale = lv_scale_create(parent);
    lv_obj_set_size(scale, 2 * RADIUS, 2 * RADIUS);
    lv_obj_center(scale);
    lv_scale_set_mode(scale, LV_SCALE_MODE_ROUND_OUTER);
    lv_scale_set_angle_range(scale, 360);
    lv_scale_set_min_value(scale, 0);
    lv_scale_set_max_value(scale, 360);
    lv_scale_set_total_tick_count(scale, 0);
    lv_scale_set_rotation(scale, 0);
    lv_obj_remove_flag(scale, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_width(scale, 30, LV_PART_MAIN);
    lv_obj_set_style_arc_color(scale, lv_color_hex(LOCK_COLOR_HEX), LV_PART_MAIN);

    float positon_rad = radians(30);

    lv_obj_t* goal_obj = lv_obj_create(scale);
    lv_obj_set_size(goal_obj, GOAL_SIZE, GOAL_SIZE);
    goal_obj_set_pos_rad(goal_obj, positon_rad);
    lv_obj_remove_flag(goal_obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(goal_obj, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(goal_obj, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(goal_obj, lv_color_hex(GOAL_COLOR_HEX), LV_PART_MAIN);
    lv_obj_set_style_border_width(goal_obj, 0, LV_PART_MAIN);

    lv_obj_t* circle_cont = lv_obj_create(parent);
    lv_obj_set_size(circle_cont, (RADIUS - WIDTH) * 2, (RADIUS - WIDTH) * 2);
    lv_obj_center(circle_cont);
    lv_obj_set_style_radius(circle_cont, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(circle_cont, lv_obj_get_style_bg_color(content, LV_PART_MAIN), LV_PART_MAIN);
    lv_obj_set_style_border_width(circle_cont, 0, LV_PART_MAIN);
    lv_obj_remove_flag(circle_cont, LV_OBJ_FLAG_CLICKABLE);

    pointer = lv_line_create(scale);
    lv_obj_set_style_line_width(pointer, POINTER_WIDTH, LV_PART_MAIN);
    lv_obj_set_style_line_color(pointer, lv_color_hex(POINTER_COLOR_HEX), LV_PART_MAIN);

    lv_obj_t* label_score = lv_label_create(parent);
    lv_obj_center(label_score);
    lv_label_set_text(label_score, "0");
    lv_obj_set_style_text_font(label_score, VW_FONT_22, LV_PART_MAIN);

    //
    pointer_set_pos_rad(scale, pointer, pointer_position);
}

//-----------------------------------------------------------------------------------------