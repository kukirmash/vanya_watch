#include "app_ui/windows/pop_the_lock.h"

#include "app_ui/ui_engine/window_manager.h"
#include "config/lcd_config.h" // VW_LCD_V_RES, VW_LCD_H_RES

#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "time.h"

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
#define GOAL_COLOR_HEX          0xEFCC3C
#define POINTER_COLOR_HEX       0xE82C5E
#define SCORE_COLOR_HEX         0xAEE7EE
#define LOCK_COLOR_HEX          0x00294D

#define CHANGE_BG_LEVEL_INTERVAL 10    
#define BG_COLORS_CNT   11
static const int BACKGROUND_COLOR_HEX[BG_COLORS_CNT] = { 0x00BC98, 0x663F90, 0xCD7B31, 0x329FB3, 0x7B7856, 0xBF58C8, 0x6DD1DA, 0x719092, 0x3A4B38, 0x0B0D30, 0xC7B136 };

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
#define POINTER_SPEED radians(130)

// Константные параметры цели
// Минимальное расстояние между указателем и появившейся целью
#define GOAL_SPAWN_DISTANCE radians(30)
// Длина дуги, на которой появляется цель
#define GOAL_SPAWN_LENGTH radians(180)
// Длина дуги, на которой будет засчитано попадание указателя в цель
#define GOAL_LENGTH  radians(15.84)

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
static game_state_t game_state = GAME_READY;
// Позиция цели в радианах
static float goal_position;

static lv_obj_t* scale = NULL;
static lv_obj_t* circle_cont = NULL;
static lv_obj_t* pointer = NULL;
static lv_obj_t* goal_obj = NULL;
static lv_obj_t* label_score = NULL;
static lv_obj_t* content = NULL;
static lv_timer_t* game_timer = NULL;

static void init_timer();

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
// Функция появления цели
static void spawn_goal()
{
    // Получение случайного расстояния на дуге
    float position = GOAL_SPAWN_LENGTH * (rand() % 1000) / 1000;
    // Обновление положения цели
    goal_position = pointer_position + pointer_direction * (GOAL_SPAWN_DISTANCE + position);

    // Установка позиции цели на экране
    goal_obj_set_pos_rad(goal_obj, goal_position);
}

//-----------------------------------------------------------------------------------------
// Функция перезапуска игры
static void game_restart()
{
    // Начальный счёт равняется уровню
    score = level;

    // Установка начального положения указателя
    pointer_position = POINTER_INITIAL_POSITION;
    pointer_set_pos_rad(scale, pointer, pointer_position);

    // Установка начального направления указателя
    pointer_direction = POINTER_INITAIL_DIRECTION;
    // Установка скорости  
    pointer_speed = POINTER_SPEED;

    uint32_t levelBgColorHex = BACKGROUND_COLOR_HEX[level / CHANGE_BG_LEVEL_INTERVAL % BG_COLORS_CNT];

    if (content == NULL || circle_cont == NULL)
        return;

    lv_obj_set_style_bg_color(content, lv_color_hex(levelBgColorHex), LV_PART_MAIN);
    lv_obj_set_style_bg_color(circle_cont, lv_color_hex(levelBgColorHex), LV_PART_MAIN);

    // Появление цели
    spawn_goal();

    // Инициализация таймера обновления состояния игры
    init_timer();
}

//-----------------------------------------------------------------------------------------
// Функция инициализации игры
static void game_init()
{
    // Игра начинается с 1 уровня
    level = 1;
    srand(time(NULL));
    game_restart();
}

//-----------------------------------------------------------------------------------------
static void game_over()
{
    if (content == NULL || circle_cont == NULL)
        return;

    lv_obj_set_style_bg_color(content, lv_color_hex(GAME_OVER_COLOR_HEX), LV_PART_MAIN);
    lv_obj_set_style_bg_color(circle_cont, lv_color_hex(GAME_OVER_COLOR_HEX), LV_PART_MAIN);
}

//-----------------------------------------------------------------------------------------
// Обновляет состояние игры
static void update(lv_timer_t* timer)
{
    const float dt = 0.033f;

    switch (game_state) {
        // Если активное состояние игры
    case GAME_POINTER_BEFORE_GOAL:
        // Обновить положение игрока
        pointer_position += dt * pointer_direction * pointer_speed;
        pointer_set_pos_rad(scale, pointer, pointer_position);

        // Если указатель попадает в цель
        if (goal_position - GOAL_LENGTH / 2 <= pointer_position && pointer_position <= goal_position + GOAL_LENGTH / 2)
            // Установить соответствующее состояние
            game_state = GAME_POINTER_INSIDE_GOAL;

        break;
    case GAME_POINTER_INSIDE_GOAL:
        // Обновить положение игрока
        pointer_position += dt * pointer_direction * pointer_speed;
        pointer_set_pos_rad(scale, pointer, pointer_position);

        // Если указатель вышел с цели
        if (goal_position - GOAL_LENGTH / 2 > pointer_position || pointer_position > goal_position + GOAL_LENGTH / 2)
        {
            // Игра проиграна
            game_state = GAME_OVER;
            game_over();
        }

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
    }
}

//-----------------------------------------------------------------------------------------
// Обработчик нажатия на экран
static void content_click_cb(lv_event_t* e)
{
    switch (game_state) {
        // Если игрок готов
    case GAME_READY:
        // Игра начинается, указатель - до цели
        game_state = GAME_POINTER_BEFORE_GOAL;
        break;

        // Если игрок нажал до цели
    case GAME_POINTER_BEFORE_GOAL:
        // Игра проиграна
        game_state = GAME_OVER;
        game_over();
        break;

        // Если игрок попал в цель
    case GAME_POINTER_INSIDE_GOAL:
        // Уменьшаем счёт
        score--;

        // Если счёт достиг нуля
        if (score <= 0)
        {
            // Достигнут следующий уровень
            level++;
            game_state = GAME_READY;
            game_restart();
        }
        else
        {
            // Игрок находится вне цели
            game_state = GAME_POINTER_BEFORE_GOAL;
            // Смена направления движения на противоположное
            pointer_direction = -pointer_direction;
            // Появляется новая цель
            spawn_goal();
        }
        break;

        // После проигрыша в игре
    case GAME_OVER:
        // Установка начального состояния игры
        game_state = GAME_READY;
        game_restart();
        break;

    default:
        break;
    }

    lv_label_set_text_fmt(label_score, "%d", score);
}

//-----------------------------------------------------------------------------------------
// Создает экран игры "Pop The Lock"
void pop_the_lock_init(lv_obj_t* parent)
{
    content = parent;
    lv_obj_set_style_bg_color(content, lv_color_hex(BACKGROUND_COLOR_HEX[0]), LV_PART_MAIN);
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

    goal_obj = lv_obj_create(scale);
    lv_obj_set_size(goal_obj, GOAL_SIZE, GOAL_SIZE);
    lv_obj_remove_flag(goal_obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(goal_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_radius(goal_obj, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(goal_obj, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(goal_obj, lv_color_hex(GOAL_COLOR_HEX), LV_PART_MAIN);
    lv_obj_set_style_border_width(goal_obj, 0, LV_PART_MAIN);

    circle_cont = lv_obj_create(parent);
    lv_obj_set_size(circle_cont, (RADIUS - WIDTH) * 2, (RADIUS - WIDTH) * 2);
    lv_obj_center(circle_cont);
    lv_obj_set_style_radius(circle_cont, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(circle_cont, lv_obj_get_style_bg_color(content, LV_PART_MAIN), LV_PART_MAIN);
    lv_obj_set_style_border_width(circle_cont, 0, LV_PART_MAIN);
    lv_obj_remove_flag(circle_cont, LV_OBJ_FLAG_CLICKABLE);

    pointer = lv_line_create(scale);
    lv_obj_set_style_line_width(pointer, POINTER_WIDTH, LV_PART_MAIN);
    lv_obj_set_style_line_color(pointer, lv_color_hex(POINTER_COLOR_HEX), LV_PART_MAIN);

    label_score = lv_label_create(parent);
    lv_obj_center(label_score);
    lv_label_set_text(label_score, "1");
    lv_obj_set_style_text_font(label_score, VW_FONT_64, LV_PART_MAIN);

    game_init();
}

//-----------------------------------------------------------------------------------------