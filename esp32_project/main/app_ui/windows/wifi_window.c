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
    // Получаем переданную точку доступа (указатель внутрь ap_list)
    const board_ap_info_t *ap = (const board_ap_info_t *)lv_event_get_user_data(e);
    if (ap == NULL)
        return;

    // Получаем Singleton конфиг
    board_wifi_config_t *cfg = board_wifi_get_config();

    // Копируем SSID в структуру (с защитой от переполнения)
    strncpy(cfg->ssid, ap->ssid, WIFI_MAX_SSID_LEN - 1);
    cfg->ssid[WIFI_MAX_SSID_LEN - 1] = '\0';

    if (ap->is_secure)
    {
        // Сеть под паролем: открываем экран ввода
        text_input_set_context(cfg->ssid, cfg->password, WIFI_MAX_PASS_LEN);
        window_open(WIN_ID_TEXT_INPUT, WIN_ANIM_SLIDE_LEFT);
    }
    else
    {
        // Открытая сеть: подключаемся сразу
        board_wifi_connect(cfg->ssid, "");
    }
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
static void create_network_row(lv_obj_t *parent, const board_ap_info_t *ap)
{
    // Строка
    lv_obj_t *row = create_transparent_row(parent);
    lv_obj_set_style_pad_ver(row, 10, LV_PART_MAIN);
    lv_obj_add_event_cb(row, network_row_clicked_cb, LV_EVENT_CLICKED, (void *)ap);

    // Название сети
    lv_obj_t *name_label = lv_label_create(row);
    lv_label_set_text(name_label, ap->ssid);
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
    if (ap->is_secure)
    {
        lv_obj_t *lock_label = lv_label_create(icons_cont);
        lv_label_set_text(lock_label, VW_SYMBOL_LOCK);
        lv_obj_set_style_text_font(lock_label, VW_FONT_18, LV_PART_MAIN);
        lv_obj_set_style_text_color(lock_label, lv_color_hex(VW_GREY_COLOR_HEX), LV_PART_MAIN);
    }

    // Иконка уровня сигнала Wi-Fi
    const char *wifi_sym = LV_SYMBOL_WIFI;
    if (ap->rssi < -75)
        wifi_sym = VW_SYMBOL_WIFI_LOW;
    else if (ap->rssi < -60)
        wifi_sym = VW_SYMBOL_WIFI_MID;

    lv_obj_t *wifi_label = lv_label_create(icons_cont);
    lv_label_set_text(wifi_label, wifi_sym);
    lv_obj_set_style_text_font(wifi_label, VW_FONT_18, LV_PART_MAIN);
    lv_obj_set_style_text_color(wifi_label, lv_color_hex(VW_PRIMARY_COLOR_HEX), LV_PART_MAIN);
}

//-----------------------------------------------------------------------------------------
// Обновляет список сетей из актуальных результатов сканирования
static lv_obj_t *s_wifi_list = NULL;
static lv_obj_t *s_status_label = NULL;
static lv_timer_t *s_refresh_timer = NULL;
static uint8_t s_last_ap_count = 255;

static void wifi_list_rebuild(lv_obj_t *list)
{
    board_wifi_config_t *cfg = board_wifi_get_config();

    lv_obj_clean(list);

    if (cfg->ap_count == 0)
    {
        lv_obj_t *no_wifi_label = lv_label_create(list);
        lv_label_set_text(no_wifi_label, "No networks found");
        lv_obj_set_style_text_color(no_wifi_label, lv_color_hex(VW_GREY_COLOR_HEX), LV_PART_MAIN);
        lv_obj_set_style_text_font(no_wifi_label, VW_FONT_18, LV_PART_MAIN);
        lv_obj_set_width(no_wifi_label, lv_pct(100));
        lv_obj_set_style_text_align(no_wifi_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    }
    else
    {
        for (int i = 0; i < cfg->ap_count; i++)
        {
            create_network_row(list, &cfg->ap_list[i]);
        }
    }

    s_last_ap_count = cfg->ap_count;
}

//-----------------------------------------------------------------------------------------
static void wifi_refresh_timer_cb(lv_timer_t *timer)
{
    (void)timer;
    board_wifi_config_t *cfg = board_wifi_get_config();

    if (s_wifi_list != NULL && cfg->ap_count != s_last_ap_count)
        wifi_list_rebuild(s_wifi_list);

    // Обновляем статус подключения
    if (s_status_label != NULL)
    {
        if (cfg->is_connected)
        {
            lv_label_set_text(s_status_label, cfg->ssid);
            lv_obj_set_style_text_color(s_status_label, lv_color_hex(VW_PRIMARY_COLOR_HEX), LV_PART_MAIN);
        }
        else
        {
            lv_label_set_text(s_status_label, "Not connected");
            lv_obj_set_style_text_color(s_status_label, lv_color_hex(VW_GREY_COLOR_HEX), LV_PART_MAIN);
        }
    }
}

//-----------------------------------------------------------------------------------------
static void wifi_window_delete_cb(lv_event_t *e)
{
    (void)e;
    if (s_refresh_timer != NULL)
    {
        lv_timer_delete(s_refresh_timer);
        s_refresh_timer = NULL;
    }
    s_wifi_list = NULL;
    s_status_label = NULL;
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

    // --- 4. СПИСОК СЕТЕЙ (реальные результаты сканирования) ---
    s_wifi_list = lv_obj_create(parent);
    lv_obj_set_width(s_wifi_list, lv_pct(100));
    lv_obj_set_layout(s_wifi_list, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(s_wifi_list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_bg_opa(s_wifi_list, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(s_wifi_list, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(s_wifi_list, 0, LV_PART_MAIN);
    lv_obj_remove_flag(s_wifi_list, LV_OBJ_FLAG_SCROLL_ELASTIC);

    wifi_list_rebuild(s_wifi_list);

    // Периодически обновляем список и статус подключения
    s_status_label = status_label;
    s_refresh_timer = lv_timer_create(wifi_refresh_timer_cb, 2000, NULL);
    lv_obj_add_event_cb(parent, wifi_window_delete_cb, LV_EVENT_DELETE, NULL);
}

//-----------------------------------------------------------------------------------------