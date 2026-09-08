#include "app_ui/windows/text_input_window.h"

#include "app_ui/ui_engine/window_manager.h"
#include "string.h"

static const char *TAG = "TEXT_INPUT_WINDOW";

//-----------------------------------------------------------------------------------------
// Глобальные переменные
static const char *target_title = "";
static char *target_buffer = NULL;
static size_t target_buffer_size = 0;

static lv_obj_t *ta;
static lv_obj_t *kb;

//-----------------------------------------------------------------------------------------
// Раскладки клавиатуры
static const char *kb_map_en[] = {
    "q", "w", "e", "r", LV_SYMBOL_BACKSPACE, "\n",
    "t", "y", "u", "i", "o", "\n",
    "p", "a", "s", "d", "f", "\n",
    "g", "h", "j", "k", "l", "\n",
    "z", "x", "c", "v", "b", "\n",
    LV_SYMBOL_UP, "n", "m", ".", ",", "\n",
    "!12", "RU", " ", LV_SYMBOL_OK, ""};

static const char *kb_map_en_up[] = {
    "Q", "W", "E", "R", LV_SYMBOL_BACKSPACE, "\n",
    "T", "Y", "U", "I", "O", "\n",
    "P", "A", "S", "D", "F", "\n",
    "G", "H", "J", "K", "L", "\n",
    "Z", "X", "C", "V", "B", "\n",
    LV_SYMBOL_DOWN, "N", "M", ".", ",", "\n",
    "!12", "RU", " ", LV_SYMBOL_OK, ""};

static const char *kb_map_ru[] = {
    "й", "ц", "у", "к", LV_SYMBOL_BACKSPACE, "\n",
    "е", "н", "г", "ш", "щ", "з", "\n",
    "х", "ъ", "ф", "ы", "в", "а", "\n",
    "п", "р", "о", "л", "д", "ж", "\n",
    "э", "я", "ч", "с", "м", "и", "\n",
    LV_SYMBOL_UP, "т", "ь", "б", "ю", "ё", "\n",
    "!12", "EN", " ", LV_SYMBOL_OK, ""};

static const char *kb_map_ru_up[] = {
    "Й", "Ц", "У", "К", LV_SYMBOL_BACKSPACE, "\n",
    "Е", "Н", "Г", "Ш", "Щ", "З", "\n",
    "Х", "Ъ", "Ф", "Ы", "В", "А", "\n",
    "П", "Р", "О", "Л", "Д", "Ж", "\n",
    "Э", "Я", "Ч", "С", "М", "И", "\n",
    LV_SYMBOL_DOWN, "Т", "Ь", "Б", "Ю", "Ё", "\n",
    "!12", "EN", " ", LV_SYMBOL_OK, ""};

static const char *kb_map_num[] = {
    "1", "2", "3", "4", LV_SYMBOL_BACKSPACE, "\n",
    "5", "6", "7", "8", "9", "\n",
    "0", "@", "#", "$", "%", "\n",
    "&", "*", "-", "+", "=", "\n",
    "/", "(", ")", "[", "]", "\n",
    ".", "?", "!", ":", ";", "\n",
    "\"", "EN", "RU", " ", LV_SYMBOL_OK, ""};

static const lv_buttonmatrix_ctrl_t custom_kb_ctrl_map[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

typedef enum
{
    KB_LANG_EN,
    KB_LANG_EN_UP,
    KB_LANG_RU,
    KB_LANG_RU_UP,
    KB_LANG_NUM
} kb_map_state_t;

static kb_map_state_t current_kb_map_state = KB_LANG_EN;

//-----------------------------------------------------------------------------------------
void text_input_set_context(const char *title, char *output_buffer, size_t buffer_size)
{
    target_title = title;
    target_buffer = output_buffer;
    target_buffer_size = buffer_size;
}

//-----------------------------------------------------------------------------------------
static void set_keyboard_map(kb_map_state_t state)
{
    current_kb_map_state = state;
    switch (state)
    {
    case KB_LANG_EN:
        lv_keyboard_set_map(kb, LV_KEYBOARD_MODE_USER_1, kb_map_en, custom_kb_ctrl_map);
        break;
    case KB_LANG_EN_UP:
        lv_keyboard_set_map(kb, LV_KEYBOARD_MODE_USER_1, kb_map_en_up, custom_kb_ctrl_map);
        break;
    case KB_LANG_RU:
        lv_keyboard_set_map(kb, LV_KEYBOARD_MODE_USER_1, kb_map_ru, custom_kb_ctrl_map);
        break;
    case KB_LANG_RU_UP:
        lv_keyboard_set_map(kb, LV_KEYBOARD_MODE_USER_1, kb_map_ru_up, custom_kb_ctrl_map);
        break;
    case KB_LANG_NUM:
        lv_keyboard_set_map(kb, LV_KEYBOARD_MODE_USER_1, kb_map_num, custom_kb_ctrl_map);
        break;
    }
}

//-----------------------------------------------------------------------------------------
static void kb_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_VALUE_CHANGED)
    {
        uint32_t btn_id = lv_keyboard_get_selected_btn(kb);
        const char *txt = lv_keyboard_get_btn_text(kb, btn_id);
        if (!txt)
            return;

        // Обработка переключения раскладок
        if (strcmp(txt, "RU") == 0)
        {
            set_keyboard_map(KB_LANG_RU);
            lv_textarea_delete_char(ta);
            lv_textarea_delete_char(ta);
        }
        else if (strcmp(txt, "EN") == 0)
        {
            set_keyboard_map(KB_LANG_EN);
            lv_textarea_delete_char(ta);
            lv_textarea_delete_char(ta);
        }
        else if (strcmp(txt, "!12") == 0)
        {
            set_keyboard_map(KB_LANG_NUM);
            lv_textarea_delete_char(ta);
            lv_textarea_delete_char(ta);
            lv_textarea_delete_char(ta);
        }
        else if (strcmp(txt, LV_SYMBOL_UP) == 0)
        {
            if (current_kb_map_state == KB_LANG_EN)
                set_keyboard_map(KB_LANG_EN_UP);
            else if (current_kb_map_state == KB_LANG_RU)
                set_keyboard_map(KB_LANG_RU_UP);
            lv_textarea_delete_char(ta);
        }
        else if (strcmp(txt, LV_SYMBOL_DOWN) == 0)
        {
            if (current_kb_map_state == KB_LANG_EN_UP)
                set_keyboard_map(KB_LANG_EN);
            else if (current_kb_map_state == KB_LANG_RU_UP)
                set_keyboard_map(KB_LANG_RU);
            lv_textarea_delete_char(ta);
        }
        else if (strcmp(txt, LV_SYMBOL_OK) == 0)
        {
            lv_textarea_delete_char(ta);
            // Кнопка галочки (или Enter) также сохраняет пароль
            if (target_buffer)
            {
                strncpy(target_buffer, lv_textarea_get_text(ta), target_buffer_size - 1);
                target_buffer[target_buffer_size - 1] = '\0';
            }
            window_back(WIN_ANIM_FADE);
        }
    }
}

//-----------------------------------------------------------------------------------------
static void btn_done_event_cb(lv_event_t *e)
{
    if (target_buffer)
    {
        strncpy(target_buffer, lv_textarea_get_text(ta), target_buffer_size - 1);
        target_buffer[target_buffer_size - 1] = '\0';
    }
    window_back(WIN_ANIM_FADE);
}

//-----------------------------------------------------------------------------------------
static void btn_back_event_cb(lv_event_t *e)
{
    window_back(WIN_ANIM_FADE);
}

//-----------------------------------------------------------------------------------------
static void btn_toggle_pwd_event_cb(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target_obj(e);
    lv_obj_t *label = lv_obj_get_child(btn, 0);
    bool is_pwd = lv_textarea_get_password_mode(ta);

    lv_textarea_set_password_mode(ta, !is_pwd);
    lv_label_set_text(label, !is_pwd ? LV_SYMBOL_EYE_CLOSE : LV_SYMBOL_EYE_OPEN);
}

//-----------------------------------------------------------------------------------------
void text_input_window_init(lv_obj_t *parent)
{
    // Layout настройки
    lv_obj_set_layout(parent, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(parent, 0, LV_PART_MAIN);

    // Верхняя панель (Заголовок и кнопка Done)
    lv_obj_t *header = lv_obj_create(parent);
    lv_obj_set_size(header, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_layout(header, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(header, 0, 0);
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_set_style_pad_all(header, 0, 0);
    lv_obj_set_style_pad_hor(header, 10, 0);
    lv_obj_set_style_pad_top(header, 10, 0);

    // Кнопка "Назад + SSID"
    lv_obj_t *back_label = lv_label_create(header);
    lv_label_set_text_fmt(back_label, LV_SYMBOL_LEFT " %s", target_title);
    lv_obj_add_flag(back_label, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(back_label, btn_back_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_text_color(back_label, lv_color_hex(VW_PRIMARY_COLOR_HEX), LV_PART_MAIN);
    lv_obj_set_style_text_font(back_label, VW_FONT_18, LV_PART_MAIN);

    // Кнопка Done
    lv_obj_t *done_label = lv_label_create(header);
    lv_label_set_text(done_label, "Done");
    lv_obj_add_flag(done_label, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(done_label, btn_done_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_text_font(done_label, VW_FONT_18, LV_PART_MAIN);

    // Контейнер поля ввода и кнопки глаза
    lv_obj_t *input_cont = lv_obj_create(parent);
    lv_obj_set_size(input_cont, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_layout(input_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(input_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(input_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(input_cont, 0, 0);
    lv_obj_set_style_border_width(input_cont, 0, 0);
    lv_obj_set_style_pad_all(input_cont, 0, 0);
    lv_obj_set_style_pad_hor(input_cont, 10, 0);

    // Поле ввода
    ta = lv_textarea_create(input_cont);
    lv_obj_set_flex_grow(ta, 1);
    lv_textarea_set_one_line(ta, true);
    lv_textarea_set_password_mode(ta, true);
    lv_textarea_set_max_length(ta, target_buffer_size - 1);
    lv_obj_set_style_border_width(ta, 0, LV_PART_MAIN);

    // Кнопка скрытия/показа пароля
    lv_obj_t *pwd_btn = lv_button_create(input_cont);
    lv_obj_t *pwd_label = lv_label_create(pwd_btn);
    lv_label_set_text(pwd_label, LV_SYMBOL_EYE_CLOSE);
    lv_obj_add_event_cb(pwd_btn, btn_toggle_pwd_event_cb, LV_EVENT_CLICKED, NULL);

    // Клавиатура
    kb = lv_keyboard_create(parent);
    lv_keyboard_set_textarea(kb, ta);
    lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_USER_1);
    lv_obj_set_flex_grow(kb, 1);
    lv_obj_add_event_cb(kb, kb_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // Установка начальной раскладки
    set_keyboard_map(KB_LANG_EN);
}

//-----------------------------------------------------------------------------------------