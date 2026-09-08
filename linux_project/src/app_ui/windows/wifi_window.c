#include "app_ui/windows/wifi_window.h"

#include "config/ui_config.h"

#include "app_ui/ui_engine/window_manager.h"
#include "app_ui/windows/text_input_window.h"

#include "board/time.h"
#include "board/wifi.h"

#include "string.h"

static const char *TAG = "WIFI_WINDOW";

//-----------------------------------------------------------------------------------------
// Обрабатывает событие нажатия на кнопку "Назад"
static void back_btn_clicked_cb(lv_event_t *e)
{
    // Возвращаемся в предыдущее окно с анимацией FADE (как и открывали)
    window_back(WIN_ANIM_FADE);
}

//-----------------------------------------------------------------------------------------
static void network_row_clicked_cb(lv_event_t * e)
{
    // Получаем переданный SSID
    const char *ssid = (const char *)lv_event_get_user_data(e);
    
    // Получаем Singleton конфиг
    wifi_config_t *cfg = board_wifi_get_config();
    
    // Копируем SSID в структуру (с защитой от переполнения)
    strncpy(cfg->ssid, ssid, WIFI_MAX_SSID_LEN - 1);
    cfg->ssid[WIFI_MAX_SSID_LEN - 1] = '\0';
    
    // Передаем заголовок и буфер для записи результата
    text_input_set_context(cfg->ssid, cfg->password, WIFI_MAX_PASS_LEN);
    
    // Открываем окно ввода
    window_open(WIN_ID_TEXT_INPUT, WIN_ANIM_SLIDE_LEFT);
}

//-----------------------------------------------------------------------------------------
// Создает прозрачный контейнер-строку, растянутый на всю ширину
static lv_obj_t *create_transparent_row(lv_obj_t *parent)
{
    lv_obj_t *row = lv_obj_create(parent);
    lv_obj_set_size(row, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_remove_flag(row, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_layout(row, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(row, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(row, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(row, 0, LV_PART_MAIN);

    return row;
}

//-----------------------------------------------------------------------------------------
// Создает строку с названием сети и иконками на правой стороне
static void create_network_row(lv_obj_t *parent, const char *ssid, bool is_locked)
{
    // Строка
    lv_obj_t *row = create_transparent_row(parent);
    lv_obj_set_style_pad_ver(row, 10, LV_PART_MAIN);
    lv_obj_add_event_cb(row, network_row_clicked_cb, LV_EVENT_CLICKED, (void *)ssid);

    // Название сети
    lv_obj_t *name_label = lv_label_create(row);
    lv_label_set_text(name_label, ssid);
    lv_obj_set_style_text_font(name_label, VW_FONT_18, LV_PART_MAIN);
    lv_obj_set_style_text_color(name_label, lv_color_white(), LV_PART_MAIN);

    // Контейнер для иконок
    lv_obj_t *icons_cont = lv_obj_create(row);
    lv_obj_set_size(icons_cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_remove_flag(icons_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_layout(icons_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(icons_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(icons_cont, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(icons_cont, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(icons_cont, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(icons_cont, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_column(icons_cont, 10, LV_PART_MAIN);

    // Иконка замка (если сеть под паролем)
    if (is_locked)
    {
        lv_obj_t *lock_label = lv_label_create(icons_cont);
        lv_label_set_text(lock_label, VW_SYMBOL_LOCK);
        lv_obj_set_style_text_font(lock_label, VW_FONT_18, LV_PART_MAIN);
        lv_obj_set_style_text_color(lock_label, lv_color_hex(VW_GREY_COLOR_HEX), LV_PART_MAIN);
    }

    // Иконка уровня сигнала Wi-Fi
    lv_obj_t *wifi_label = lv_label_create(icons_cont);
    lv_label_set_text(wifi_label, LV_SYMBOL_WIFI);
    lv_obj_set_style_text_font(wifi_label, VW_FONT_18, LV_PART_MAIN);
    lv_obj_set_style_text_color(wifi_label, lv_color_hex(VW_PRIMARY_COLOR_HEX), LV_PART_MAIN);
}

//-----------------------------------------------------------------------------------------
void wifi_window_init(lv_obj_t *parent)
{
    // Настраиваем родительский контейнер как вертикальный Flex
    lv_obj_set_layout(parent, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_remove_flag(parent, LV_OBJ_FLAG_SCROLL_ELASTIC);

    // Отступы от краев экрана
    lv_obj_set_style_pad_hor(parent, 15, LV_PART_MAIN);
    lv_obj_set_style_pad_top(parent, 20, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(parent, 20, LV_PART_MAIN);
    lv_obj_set_style_pad_row(parent, 10, LV_PART_MAIN);

    // 1. ЗАГОЛОВОК И ВРЕМЯ
    lv_obj_t *header_row = create_transparent_row(parent);
    lv_obj_set_style_margin_bottom(header_row, 5, LV_PART_MAIN);

    lv_obj_t *back_label = lv_label_create(header_row);
    lv_label_set_text(back_label, LV_SYMBOL_LEFT " Wi-Fi");
    lv_obj_add_flag(back_label, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_ext_click_area(back_label, 15);
    lv_obj_add_event_cb(back_label, back_btn_clicked_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_text_color(back_label, lv_color_hex(VW_PRIMARY_COLOR_HEX), LV_PART_MAIN); // Оранжевый
    lv_obj_set_style_text_font(back_label, VW_FONT_18, LV_PART_MAIN);

    lv_obj_t *time_label = lv_label_create(header_row);
    lv_obj_set_style_text_color(time_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(time_label, VW_FONT_18, LV_PART_MAIN);
    lv_label_bind_text(time_label, &subject_str_time, NULL);

    // 2. ПЕРЕКЛЮЧАТЕЛЬ WI-FI
    lv_obj_t *toggle_row = create_transparent_row(parent);

    lv_obj_t *wifi_title = lv_label_create(toggle_row);
    lv_label_set_text(wifi_title, "Wi-Fi");
    lv_obj_set_style_text_color(wifi_title, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(wifi_title, VW_FONT_22, LV_PART_MAIN);

    lv_obj_t *wifi_switch = lv_switch_create(toggle_row);
    lv_obj_add_state(wifi_switch, LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(wifi_switch, lv_color_hex(VW_PRIMARY_COLOR_HEX), LV_PART_INDICATOR | LV_STATE_CHECKED);

    // Статус подключения
    lv_obj_t *status_label = lv_label_create(parent);
    lv_label_set_text(status_label, "Not connected");
    lv_obj_set_style_text_color(status_label, lv_color_hex(VW_GREY_COLOR_HEX), LV_PART_MAIN);
    lv_obj_set_style_text_font(status_label, VW_FONT_14, LV_PART_MAIN);
    lv_obj_set_style_margin_bottom(status_label, 15, LV_PART_MAIN); // Добавляем отступ перед списком сетей

    // 3. ЗАГОЛОВОК СПИСКА СЕТЕЙ
    lv_obj_t *choose_label = lv_label_create(parent);
    lv_obj_set_width(choose_label, lv_pct(100));
    lv_label_set_text(choose_label, "CHOOSE NETWORK");
    lv_obj_set_style_text_color(choose_label, lv_color_hex(VW_GREY_COLOR_HEX), LV_PART_MAIN);
    lv_obj_set_style_text_font(choose_label, VW_FONT_14, LV_PART_MAIN);
    lv_obj_set_style_text_align(choose_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);

    // --- 4. СПИСОК СЕТЕЙ (Примеры) ---
    create_network_row(parent, "anuta", true);
    create_network_row(parent, "Keenetic-7722", true);
    create_network_row(parent, "Guest_WiFi", false);
}

//-----------------------------------------------------------------------------------------