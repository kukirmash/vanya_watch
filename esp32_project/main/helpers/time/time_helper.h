#ifndef TIME_HELPER_H
#define TIME_HELPER_H

#include "stdint.h"

//-----------------------------------------------------------------------------------------
// Вспомогательные функции для вычисления предыдущего значения 

// Для единиц минут (0..9 -> предыдущее 9)
uint8_t get_prev_m_units( uint8_t current );

// Для десятков минут (0..5 -> предыдущее 5)
uint8_t get_prev_m_tens( uint8_t current );

// Для десятков часов (0..2 -> предыдущее 2)
uint8_t get_prev_h_tens( uint8_t current );

// Для единиц часов  (переход полночи vs обычный десяток)
uint8_t get_prev_h_units( uint8_t current, uint8_t new_h_tens );

//-----------------------------------------------------------------------------------------

#endif // TIME_HELPER_H