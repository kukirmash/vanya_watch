#define M_PI 3.14159265358979323846 // pi
#define radians(degrees) ((degrees) * M_PI / 180);

#include "game.h"
#include <stdlib.h>

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
// Состояние анимации
typedef enum {
    ANIM_NONE = 0,          // Анимации нет
    ANIM_SPAWN_GOAL = 1,    // Анимация появления цели
    ANIM_SHAKE_LOCK = 2,    // Анимация тряски замка
    ANIM_OPEN_LOCK = 3,     // Анимация открытия замка
    ANIM_MOVE_OUT_LOCK = 4, // Анимация, когда замок уезжает за экран
    ANIM_MOVE_IN_LOCK = 5,  // Анимация, когда замок приезжает обратно

    ANIM_COUNT
} anim_state_t;

//-----------------------------------------------------------------------------------------
// Направление движения указателя
typedef enum {
    CLOCKWISE = -1,       // По часовой стрелке
    COUNTERCLOCKWISE = 1  // Против часовой стрелки
} pointer_dir_t;

//-----------------------------------------------------------------------------------------
// Скорость анимаций
const float ANIM_SPEED[] = {
    0.0, // ANIM_NONE
    0.1, // ANIM_SPAWN_GOAL
    0.1, // ANIM_SHAKE_LOCK
    0.3, // ANIM_OPEN_LOCK
    0.3, // ANIM_MOVE_OUT_LOCK
    0.3, // ANIM_MOVE_IN_LOCK
};

//-----------------------------------------------------------------------------------------
// Константные параметры указателя
// Начальное положение указателя
const float POINTER_INITIAL_POSITION = radians(90);
// Начальное направление указателя
const pointer_dir_t POINTER_INITAIL_DIRECTION = COUNTERCLOCKWISE;

//-----------------------------------------------------------------------------------------
// Константные параметры цели
// Минимальное расстояние между указателем и появившейся целью
const float GOAL_SPAWN_DISTANCE = radians(30);
// Длина дуги, на которой появляется цель
const float GOAL_SPAWN_LENGTH = radians(180);
// Длина дуги, на которой будет засчитано попадание указателя в цель
const float GOAL_LENGTH = radians(10);

//-----------------------------------------------------------------------------------------
// Текущий уровень
int level;
// Текущий счёт
int score;
// Положение указателя в радианах
float pointer_position;
// Направление указателя
pointer_dir_t pointer_direction;
// Скорость указателя (рад/с)
float pointer_speed;

// Состояние игры
game_state_t game_state;
// Состояние анимации
anim_state_t anim_state;
// Прогресс анимации 0..1
float anim_progress;
// Позиция цели в радианах
float goal_position;

//-----------------------------------------------------------------------------------------
// Функция инициализации игры
void init()
{
    // Игра начинается с 1 уровня
    level = 1;
    restart();
}

//-----------------------------------------------------------------------------------------
// Функция перезапуска игры
void restart()
{
    // Начальный счёт равняется уровню
    score = level;

    // Установка начального положения указателя
    pointer_position = POINTER_INITIAL_POSITION;
    // Установка начального направления указателя
    pointer_direction = POINTER_INITAIL_DIRECTION;
    // Установка скорости TODO переделать 
    pointer_speed = radians(75 + level);

    // Установка начального состояния игры
    game_state = GAME_READY;
    // Активация анимации появления цели
    anim_state = ANIM_SPAWN_GOAL;
    // Прогресс начинается с 0
    anim_progress = 0.0;

    // Появление цели
    spawn_goal();
}

//-----------------------------------------------------------------------------------------
// Функция появления цели
void spawn_goal()
{
    // Получение случайного расстояния на дуге
    float position = GOAL_SPAWN_LENGTH * (rand() % 1000) / 1000;
    // Обновление положения цели
    goal_position = pointer_position + pointer_direction * (GOAL_SPAWN_DISTANCE + position);
    // Запуск анимации появления цели
    anim_state = ANIM_SPAWN_GOAL;
}

//-----------------------------------------------------------------------------------------
// Функция обновления состояния игры на 1 кадр
void update(float dt)
{
    switch (game_state) {
        // Если активное состояние игры
    case GAME_POINTER_BEFORE_GOAL:
    case GAME_POINTER_INSIDE_GOAL:
        // Обновить положение игрока
        pointer_position += dt * pointer_direction * pointer_speed;

        // Если указатель попадает в цель
        if (goal_position - GOAL_LENGTH / 2 <= pointer_position
            && pointer_position <= goal_position + GOAL_LENGTH / 2) {
            // Установить соответствующее состояние
            game_state = GAME_POINTER_INSIDE_GOAL;
        }

        // Если указатель вышел с цели
        else if (game_state == GAME_POINTER_INSIDE_GOAL) {
            // Игра проиграна
            game_state = GAME_OVER;
        }

        break;
    }

    // Продвижение анимации
    anim_progress += dt * ANIM_SPEED[anim_state];

    // Если анимация завершилась
    if (anim_progress >= 1.0) {
        // Сбросить прогресс
        anim_progress = 0.0;

        // Запуск следующей анимации
        switch (anim_state) {
        case ANIM_OPEN_LOCK:
            anim_state = ANIM_MOVE_OUT_LOCK;
            break;

        case ANIM_MOVE_OUT_LOCK:
            anim_state = ANIM_MOVE_IN_LOCK;
            break;

        default:
            anim_state = ANIM_NONE;
            break;
        }
    }
}

//-----------------------------------------------------------------------------------------
// Функция обработки нажатия игроком
void tap()
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
        break;

        // Если игрок попал в цель
    case GAME_POINTER_INSIDE_GOAL:
        // Уменьшаем счёт
        score--;

        // Если счёт достиг нуля
        if (score <= 0) {
            // Достигнут следующий уровень
            level++;
        }
        else {
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
        // Запустить новую
        restart();
        break;
    }
}

//-----------------------------------------------------------------------------------------