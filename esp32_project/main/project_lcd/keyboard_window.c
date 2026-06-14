#include "keyboard_window.h"

#if MOD_LVGL_LCD && VW_WORK_MODE

#include <string.h>
#include <stdio.h>

//-----------------------------------------------------------------------------------------
static lv_obj_t* kb_window = NULL;
static lv_obj_t* text_area = NULL;
static lv_obj_t* keyboard = NULL;

static char* target_buffer = NULL;
static size_t target_buf_size = 0;
static keyboard_done_cb on_done_cb = NULL;
static kb_input_type current_type;

static bool is_caps = false;
static uint8_t current_lang = 0; // 0 = EN, 1 = RU, 2 = SYM

// Флаг для системных кнопок (отключает залипание и выделяет кнопку визуально)
#define KB_CTRL_BTN (LV_BUTTONMATRIX_CTRL_NO_REPEAT)

//-----------------------------------------------------------------------------------------
// 1. АНГЛИЙСКИЕ РАСКЛАДКИ (7 строк)
static const char* kb_map_en_lc[] = {
    "q", "w", "e", "r", LV_SYMBOL_BACKSPACE, "\n",
    "t", "y", "u", "i", "o", "\n",
    "p", "a", "s", "d", "f", "\n",
    "g", "h", "j", "k", "l", "\n",
    "z", "x", "c", "v", "b", "\n",
    LV_SYMBOL_UP, "n", "m", ".", ",", "\n",
    "!12", "RU", " ", LV_SYMBOL_OK, ""
};
static const char* kb_map_en_uc[] = {
    "Q", "W", "E", "R", LV_SYMBOL_BACKSPACE, "\n",
    "T", "Y", "U", "I", "O", "\n",
    "P", "A", "S", "D", "F", "\n",
    "G", "H", "J", "K", "L", "\n",
    "Z", "X", "C", "V", "B", "\n",
    LV_SYMBOL_UP, "N", "M", ".", ",", "\n",
    "!12", "RU", " ", LV_SYMBOL_OK, ""
};
static const lv_buttonmatrix_ctrl_t kb_ctrl_en[] = {
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    KB_CTRL_BTN | 2, 2, 2, 2, 2,
    KB_CTRL_BTN | 2, KB_CTRL_BTN | 2, 4, KB_CTRL_BTN | 2
};

//-----------------------------------------------------------------------------------------
// 2. РУССКИЕ РАСКЛАДКИ (7 строк)
static const char* kb_map_ru_lc[] = {
    "й", "ц", "у", "к", LV_SYMBOL_BACKSPACE, "\n",
    "е", "н", "г", "ш", "щ", "з", "\n",
    "х", "ъ", "ф", "ы", "в", "а", "\n",
    "п", "р", "о", "л", "д", "ж", "\n",
    "э", "я", "ч", "с", "м", "и", "\n",
    LV_SYMBOL_UP, "т", "ь", "б", "ю", "ё", "\n",
    "!12", "EN", " ", LV_SYMBOL_OK, ""
};
static const char* kb_map_ru_uc[] = {
    "Й", "Ц", "У", "К", LV_SYMBOL_BACKSPACE, "\n",
    "Е", "Н", "Г", "Ш", "Щ", "З", "\n",
    "Х", "Ъ", "Ф", "Ы", "В", "А", "\n",
    "П", "Р", "О", "Л", "Д", "Ж", "\n",
    "Э", "Я", "Ч", "С", "М", "И", "\n",
    LV_SYMBOL_UP, "Т", "Ь", "Б", "Ю", "Ё", "\n",
    "!12", "EN", " ", LV_SYMBOL_OK, ""
};
static const lv_buttonmatrix_ctrl_t kb_ctrl_ru[] = {
    1, 1, 1, 1, KB_CTRL_BTN | 2,
    1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1,
    KB_CTRL_BTN | 1, 1, 1, 1, 1, 1, // Caps шириной 1, как и остальные 5 букв
    KB_CTRL_BTN | 2, KB_CTRL_BTN | 2, 4, KB_CTRL_BTN | 2
};

//-----------------------------------------------------------------------------------------
// 3. СИМВОЛЬНАЯ РАСКЛАДКА С ЦИФРАМИ (7 строк)
static const char* kb_map_sym[] = {
    "1", "2", "3", "4", LV_SYMBOL_BACKSPACE, "\n",
    "5", "6", "7", "8", "9", "\n",
    "0", "@", "#", "$", "%", "\n",
    "&", "*", "-", "+", "=", "\n",
    "/", "(", ")", "[", "]", "\n",
    "_", "?", "!", ":", ";", "\n",
    "\"", "EN", "RU", " ", LV_SYMBOL_OK, ""
};
static const lv_buttonmatrix_ctrl_t kb_ctrl_sym[] = {
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 3, KB_CTRL_BTN | 2 // Настроил ширину для добавленной кавычки
};

//-----------------------------------------------------------------------------------------
static void close_keyboard_window( void )
{
    if ( kb_window )
    {
        lv_obj_delete_async( kb_window );
        kb_window = NULL;
    }
}

//-----------------------------------------------------------------------------------------
// Обработчики кнопок шапки (Cancel / Done)
static void header_btn_event_cb( lv_event_t* e )
{
    lv_event_code_t code = lv_event_get_code( e );
    intptr_t action = (intptr_t)lv_event_get_user_data( e );

    if ( code == LV_EVENT_CLICKED )
    {
        if ( action == 1 )
        { // Нажали Done
            if ( target_buffer )
            {
                strncpy( target_buffer, lv_textarea_get_text( text_area ), target_buf_size - 1 );
                target_buffer[ target_buf_size - 1 ] = '\0';
            }

            if ( on_done_cb )
                on_done_cb( target_buffer );
        }
        close_keyboard_window();
    }
}

//-----------------------------------------------------------------------------------------
// Обработчик кнопки Глаза (Показать/Скрыть пароль)
static void eye_btn_event_cb( lv_event_t* e )
{
    if ( lv_event_get_code( e ) == LV_EVENT_CLICKED )
    {
        lv_obj_t* btn = lv_event_get_target( e );
        lv_obj_t* label = lv_obj_get_child( btn, 0 );

        bool is_password_mode = lv_textarea_get_password_mode( text_area );
        lv_textarea_set_password_mode( text_area, !is_password_mode );

        lv_label_set_text( label, is_password_mode ? LV_SYMBOL_EYE_OPEN : LV_SYMBOL_EYE_CLOSE );
    }
}

//-----------------------------------------------------------------------------------------
// Универсальный обработчик кастомной клавиатуры
static void keyboard_event_cb( lv_event_t* e )
{
    if ( lv_event_get_code( e ) == LV_EVENT_VALUE_CHANGED )
    {
        uint32_t btn_id = lv_keyboard_get_selected_btn( keyboard );
        const char* txt = lv_keyboard_get_btn_text( keyboard, btn_id );

        if ( !txt )
            return;

        // 1. Отрабатываем галочку (LV_SYMBOL_OK) как кнопку Done
        if ( strcmp( txt, LV_SYMBOL_OK ) == 0 )
        {
            lv_textarea_delete_char( text_area ); // Удаляем напечатанный символ галочки

            if ( target_buffer )
            {
                strncpy( target_buffer, lv_textarea_get_text( text_area ), target_buf_size - 1 );
                target_buffer[ target_buf_size - 1 ] = '\0';
            }
            if ( on_done_cb )
                on_done_cb( target_buffer );
            close_keyboard_window();
            return;
        }

        // 2. Обработка Caps Lock
        if ( strcmp( txt, LV_SYMBOL_UP ) == 0 )
        {
            lv_textarea_delete_char( text_area ); // Удаляем стрелку из текста
            is_caps = !is_caps;

            if ( current_lang == 0 )
                lv_keyboard_set_map( keyboard, LV_KEYBOARD_MODE_USER_1, is_caps ? kb_map_en_uc : kb_map_en_lc, kb_ctrl_en );
            else if ( current_lang == 1 )
                lv_keyboard_set_map( keyboard, LV_KEYBOARD_MODE_USER_1, is_caps ? kb_map_ru_uc : kb_map_ru_lc, kb_ctrl_ru );
            return;
        }

        // 3. Обрабатываем переключение раскладок
        int chars_to_delete = 0;
        if ( strcmp( txt, "RU" ) == 0 )
            chars_to_delete = 2;
        else if ( strcmp( txt, "EN" ) == 0 )
            chars_to_delete = 2;
        else if ( strcmp( txt, "!12" ) == 0 )
            chars_to_delete = 3;

        if ( chars_to_delete > 0 )
        {
            // Стираем из поля ввода символы системных кнопок ("RU", "EN" и тд.)
            for ( int i = 0; i < chars_to_delete; i++ )
            {
                lv_textarea_delete_char( text_area );
            }

            // Меняем раскладку клавиатуры
            if ( strcmp( txt, "RU" ) == 0 )
            {
                current_lang = 1;
                lv_keyboard_set_map( keyboard, LV_KEYBOARD_MODE_USER_1, is_caps ? kb_map_ru_uc : kb_map_ru_lc, kb_ctrl_ru );
            }
            else if ( strcmp( txt, "EN" ) == 0 )
            {
                current_lang = 0;
                lv_keyboard_set_map( keyboard, LV_KEYBOARD_MODE_USER_1, is_caps ? kb_map_en_uc : kb_map_en_lc, kb_ctrl_en );
            }
            else if ( strcmp( txt, "!12" ) == 0 )
            {
                current_lang = 2;
                lv_keyboard_set_map( keyboard, LV_KEYBOARD_MODE_USER_1, kb_map_sym, kb_ctrl_sym );
            }
        }
    }
}

//-----------------------------------------------------------------------------------------
void keyboard_window_create( const char* title, char* out_buffer, size_t buf_size, kb_input_type type, keyboard_done_cb done_cb )
{
    if ( kb_window != NULL )
        return;

    target_buffer = out_buffer;
    target_buf_size = buf_size;
    on_done_cb = done_cb;
    current_type = type;

    // Сбрасываем состояния при новом открытии
    is_caps = false;
    current_lang = 0;

    lv_obj_t* parent = lv_screen_active();
    kb_window = lv_obj_create( parent );
    lv_obj_set_size( kb_window, lv_pct( 100 ), lv_pct( 100 ) );
    lv_obj_set_layout( kb_window, LV_LAYOUT_FLEX );
    lv_obj_set_flex_flow( kb_window, LV_FLEX_FLOW_COLUMN );
    lv_obj_set_style_bg_color( kb_window, lv_color_black(), LV_PART_MAIN );
    lv_obj_set_style_pad_all( kb_window, 0, LV_PART_MAIN );
    lv_obj_set_style_border_width( kb_window, 0, LV_PART_MAIN );

    // Шапка
    lv_obj_t* header = lv_obj_create( kb_window );
    lv_obj_set_size( header, lv_pct( 100 ), 30 );
    lv_obj_remove_flag( header, LV_OBJ_FLAG_SCROLLABLE );
    lv_obj_set_style_bg_opa( header, 0, LV_PART_MAIN );
    lv_obj_set_style_border_width( header, 0, LV_PART_MAIN );
    lv_obj_set_style_pad_ver( header, 0, LV_PART_MAIN );
    lv_obj_set_style_pad_hor( header, 5, LV_PART_MAIN );

    // Формируем текст: < Название (первые 6 букв)
    char cancel_text[ 32 ];
    if ( strlen( title ) > 6 )
        snprintf( cancel_text, sizeof( cancel_text ), LV_SYMBOL_LEFT " %.6s...", title );
    else
        snprintf( cancel_text, sizeof( cancel_text ), LV_SYMBOL_LEFT " %s", title );

    // Левая кнопка (Cancel)
    lv_obj_t* cancel_btn = lv_button_create( header );
    lv_obj_align( cancel_btn, LV_ALIGN_LEFT_MID, 0, 0 );
    lv_obj_add_event_cb( cancel_btn, header_btn_event_cb, LV_EVENT_CLICKED, (void*)0 );
    lv_obj_set_style_bg_opa( cancel_btn, 0, LV_PART_MAIN );
    lv_obj_set_style_shadow_width( cancel_btn, 0, LV_PART_MAIN );
    lv_obj_t* cancel_lbl = lv_label_create( cancel_btn );
    lv_label_set_text( cancel_lbl, cancel_text );
    lv_obj_set_style_text_color( cancel_lbl, lv_color_hex( VW_PRIMARY_COLOR_HEX ), LV_PART_MAIN );
    lv_obj_set_style_text_font( cancel_lbl, VW_FONT_18, LV_PART_MAIN );

    // Правая кнопка (Done)
    lv_obj_t* done_btn = lv_button_create( header );
    lv_obj_align( done_btn, LV_ALIGN_RIGHT_MID, 0, 0 );
    lv_obj_add_event_cb( done_btn, header_btn_event_cb, LV_EVENT_CLICKED, (void*)1 );
    lv_obj_set_style_bg_opa( done_btn, 0, LV_PART_MAIN );
    lv_obj_set_style_shadow_width( done_btn, 0, LV_PART_MAIN );
    lv_obj_t* done_lbl = lv_label_create( done_btn );
    lv_label_set_text( done_lbl, "Done" );
    lv_obj_set_style_text_color( done_lbl, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font( done_lbl, VW_FONT_18, LV_PART_MAIN );

    // Поле ввода и глаз
    lv_obj_t* input_cont = lv_obj_create( kb_window );
    lv_obj_set_size( input_cont, lv_pct( 100 ), LV_SIZE_CONTENT );
    lv_obj_set_layout( input_cont, LV_LAYOUT_FLEX );
    lv_obj_set_flex_flow( input_cont, LV_FLEX_FLOW_ROW );
    lv_obj_set_flex_align( input_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER );
    lv_obj_set_style_bg_opa( input_cont, 0, LV_PART_MAIN );
    lv_obj_set_style_border_width( input_cont, 0, LV_PART_MAIN );
    lv_obj_set_style_pad_ver( input_cont, 0, LV_PART_MAIN );
    lv_obj_set_style_pad_hor( input_cont, 5, LV_PART_MAIN );

    text_area = lv_textarea_create( input_cont );
    lv_obj_set_flex_grow( text_area, 1 );
    lv_textarea_set_one_line( text_area, true );
    lv_obj_set_style_bg_color( text_area, lv_color_hex( VW_DARK_GREY_COLOR_HEX ), LV_PART_MAIN );
    lv_obj_set_style_text_color( text_area, lv_color_white(), LV_PART_MAIN );
    lv_obj_set_style_border_width( text_area, 0, LV_PART_MAIN );

    // Настройка типов ввода
    if ( type == KB_INPUT_PASSWORD )
    {
        lv_textarea_set_password_mode( text_area, true );

        lv_obj_t* eye_btn = lv_button_create( input_cont );
        lv_obj_set_style_bg_color( eye_btn, lv_color_hex( 0x333333 ), LV_PART_MAIN );
        lv_obj_add_event_cb( eye_btn, eye_btn_event_cb, LV_EVENT_CLICKED, NULL );
        lv_obj_t* eye_lbl = lv_label_create( eye_btn );
        lv_label_set_text( eye_lbl, LV_SYMBOL_EYE_CLOSE );
    }
    else if ( type == KB_INPUT_NUM_ONLY)
    {
        lv_textarea_set_accepted_chars( text_area, "0123456789" );
    }
    else if ( type == KB_INPUT_PIN5 )
    {
        lv_textarea_set_accepted_chars( text_area, "0123456789" );
        lv_textarea_set_max_length( text_area, 5 );
    }

    // Клавиатура
    keyboard = lv_keyboard_create( kb_window );
    lv_obj_set_flex_grow( keyboard, 1 );
    lv_keyboard_set_textarea( keyboard, text_area );
    lv_obj_add_event_cb( keyboard, keyboard_event_cb, LV_EVENT_VALUE_CHANGED, NULL );

    // Активируем кастомную английскую раскладку по умолчанию
    if ( type == KB_INPUT_TEXT || type == KB_INPUT_PASSWORD )
    {
        lv_keyboard_set_map( keyboard, LV_KEYBOARD_MODE_USER_1, kb_map_en_lc, kb_ctrl_en );
        lv_keyboard_set_mode( keyboard, LV_KEYBOARD_MODE_USER_1 );
    }
}

//-----------------------------------------------------------------------------------------
#endif // MOD_LVGL_LCD && VW_WORK_MODE
