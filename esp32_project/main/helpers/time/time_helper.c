#include "helpers/time/time_helper.h"

//-----------------------------------------------------------------------------------------
// Вспомогательные функции для вычисления предыдущего значения десятки/единицы часа/минуты

// Для единиц минут (0..9 -> предыдущее 9)
uint8_t get_prev_m_units(uint8_t current)
{
    return (current == 0) ? 9 : current - 1;
}

// Для десятков минут (0..5 -> предыдущее 5)
uint8_t get_prev_m_tens(uint8_t current)
{
    return (current == 0) ? 5 : current - 1;
}

// Для десятков часов (0..2 -> предыдущее 2)
uint8_t get_prev_h_tens(uint8_t current)
{
    return (current == 0) ? 2 : current - 1;
}

// Для единиц часов
uint8_t get_prev_h_units(uint8_t current_h_unit, uint8_t current_h_tens)
{
    // Если сейчас 00:xx (десятки равны 0), то прошлый час был 23:xx (единица = 3).
    // Для 10:xx или 20:xx прошлый час заканчивался на 9 (09:xx или 19:xx).
    return (current_h_unit == 0) ? ((current_h_tens == 0) ? 3 : 9) : current_h_unit - 1;
}

//-----------------------------------------------------------------------------------------