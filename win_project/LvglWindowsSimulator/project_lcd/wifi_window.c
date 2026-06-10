#include "wifi_window.h"
#include "project_lcd_main.h"

#if MOD_LVGL_LCD && VW_WORK_MODE

//-----------------------------------------------------------------------------------------
#include "../config/project_config.h"

// Подключаем функционал Wi-Fi только для ESP
#if ESP32
#include "wifi.h"
#endif

static const char* TAG = "VW_WIFI_WINDOW";

static lv_obj_t* wifi_window = NULL;

//-----------------------------------------------------------------------------------------
// Обработчик кнопки "Назад"
static void back_btn_event_cb( lv_event_t* e )
{
    lv_event_code_t code = lv_event_get_code( e );
    if (code == LV_EVENT_CLICKED && wifi_window != NULL)
    {
        // Плавно удаляем экран со списком
        lv_obj_delete_async( wifi_window );
        wifi_window = NULL;
    }
}

//-----------------------------------------------------------------------------------------
// Вспомогательная функция для создания одного пункта списка сетей
static lv_obj_t* create_wifi_list_item( lv_obj_t* parent, const char* ssid, bool is_secure, int rssi )
{
    lv_obj_t* item = lv_button_create( parent );
    lv_obj_set_width( item, lv_pct( 100 ) );
    lv_obj_set_height( item, LV_SIZE_CONTENT );
    lv_obj_set_style_bg_color( item, lv_color_hex( VW_DARK_GREY_COLOR_HEX ), LV_PART_MAIN ); // Темно-серый цвет
    lv_obj_set_style_radius( item, 16, LV_PART_MAIN );
    lv_obj_set_style_pad_all( item, 15, LV_PART_MAIN );

    // Включаем Flex-layout для выравнивания элементов внутри плашки
    lv_obj_set_layout( item, LV_LAYOUT_FLEX );
    lv_obj_set_flex_flow( item, LV_FLEX_FLOW_ROW );
    lv_obj_set_flex_align( item, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER );

    // Название сети (SSID)
    lv_obj_t* ssid_label = lv_label_create( item );
    lv_label_set_text( ssid_label, ssid );
    lv_obj_set_style_text_color( ssid_label, lv_color_white(), LV_PART_MAIN );
    lv_obj_set_style_text_font( ssid_label, VW_FONT_18, LV_PART_MAIN );
    lv_obj_set_flex_grow( ssid_label, 1 ); // Занимает всё доступное пространство слева

    // Иконка замка (если сеть под паролем)
    if (is_secure) {
        lv_obj_t* lock_icon = lv_label_create( item );
        lv_label_set_text( lock_icon, LV_SYMBOL_WARNING ); // TODO: Заменить на кастомный замок, если есть в шрифте
        lv_obj_set_style_text_color( lock_icon, lv_color_hex( VW_GREY_COLOR_HEX ), LV_PART_MAIN );
    }

    // Иконка сигнала Wi-Fi
    lv_obj_t* signal_icon = lv_label_create( item );
    lv_label_set_text( signal_icon, LV_SYMBOL_WIFI );
    lv_obj_set_style_text_color( signal_icon, lv_color_white(), LV_PART_MAIN );

    return item;
}

//-----------------------------------------------------------------------------------------
void wifi_window_create( void )
{
    if (wifi_window != NULL)
        return; // Защита от двойного открытия

    // 1. Создаем контейнер, который перекроет всё
    lv_obj_t* parent = lv_screen_active();
    wifi_window = lv_obj_create( parent );
    lv_obj_set_size( wifi_window, lv_pct( 100 ), lv_pct( 100 ) );
    lv_obj_set_style_bg_color( wifi_window, lv_color_black(), LV_PART_MAIN );
    lv_obj_set_style_border_width( wifi_window, 0, LV_PART_MAIN );
    lv_obj_set_style_radius( wifi_window, 0, LV_PART_MAIN );
    lv_obj_set_style_pad_all( wifi_window, 10, LV_PART_MAIN );
    lv_obj_set_scrollbar_mode( wifi_window, LV_SCROLLBAR_MODE_OFF );

    // Включаем Flex-колонку для всего экрана
    lv_obj_set_layout( wifi_window, LV_LAYOUT_FLEX );
    lv_obj_set_flex_flow( wifi_window, LV_FLEX_FLOW_COLUMN );
    lv_obj_set_flex_align( wifi_window, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER );

    // 2. Шапка (Header) - Назад и Время
    lv_obj_t* header = lv_obj_create( wifi_window );
    lv_obj_set_width( header, lv_pct( 100 ) );
    lv_obj_set_height( header, LV_SIZE_CONTENT );
    lv_obj_set_style_bg_opa( header, 0, LV_PART_MAIN );
    lv_obj_set_style_border_width( header, 0, LV_PART_MAIN );
    lv_obj_set_style_pad_all( header, 5, LV_PART_MAIN );

    lv_obj_t* back_btn = lv_btn_create( header );
    lv_obj_set_style_bg_opa( back_btn, 0, LV_PART_MAIN );
    lv_obj_set_style_shadow_width( back_btn, 0, LV_PART_MAIN );
    lv_obj_align( back_btn, LV_ALIGN_LEFT_MID, 0, 0 );
    lv_obj_add_event_cb( back_btn, back_btn_event_cb, LV_EVENT_CLICKED, NULL );

    lv_obj_t* back_label = lv_label_create( back_btn );
    lv_label_set_text( back_label, LV_SYMBOL_LEFT " Wi-Fi" );
    lv_obj_set_style_text_color( back_label, lv_color_hex( VW_PRIMARY_COLOR_HEX ), LV_PART_MAIN );
    lv_obj_set_style_text_font( back_label, VW_FONT_18, LV_PART_MAIN );

    lv_obj_t* time_label = lv_label_create( header );
    lv_label_set_text( time_label, "12:57" ); // TODO: Взять реальное время через get_curr_time()
    lv_obj_align( time_label, LV_ALIGN_RIGHT_MID, 0, 0 );
    lv_obj_set_style_text_color( time_label, lv_color_white(), LV_PART_MAIN );
    lv_obj_set_style_text_font( time_label, VW_FONT_18, LV_PART_MAIN );

    // 3. Главный переключатель Wi-Fi
    lv_obj_t* main_toggle_cont = lv_obj_create( wifi_window );
    lv_obj_set_width( main_toggle_cont, lv_pct( 100 ) );
    lv_obj_set_height( main_toggle_cont, LV_SIZE_CONTENT );
    lv_obj_set_style_bg_color( main_toggle_cont, lv_color_hex( VW_DARK_GREY_COLOR_HEX ), LV_PART_MAIN );
    lv_obj_set_style_border_width( main_toggle_cont, 0, LV_PART_MAIN );
    lv_obj_set_style_radius( main_toggle_cont, 16, LV_PART_MAIN );

    lv_obj_t* toggle_label = lv_label_create( main_toggle_cont );
    lv_label_set_text( toggle_label, "Wi-Fi" );
    lv_obj_align( toggle_label, LV_ALIGN_LEFT_MID, 5, 0 );
    lv_obj_set_style_text_color( toggle_label, lv_color_white(), LV_PART_MAIN );
    lv_obj_set_style_text_font( toggle_label, VW_FONT_18, LV_PART_MAIN );

    lv_obj_t* sw = lv_switch_create( main_toggle_cont );
    lv_obj_align( sw, LV_ALIGN_RIGHT_MID, -5, 0 );
    lv_obj_add_state( sw, LV_STATE_CHECKED ); // По умолчанию включено
    lv_obj_set_style_bg_color( sw, lv_color_hex( VW_PRIMARY_COLOR_HEX ), LV_PART_INDICATOR | LV_STATE_CHECKED );

    // 4. Заголовок "CHOOSE NETWORK"
    lv_obj_t* section_label = lv_label_create( wifi_window );
    lv_label_set_text( section_label, "CHOOSE NETWORK" );
    lv_obj_set_style_text_color( section_label, lv_color_hex( VW_GREY_COLOR_HEX ), LV_PART_MAIN ); // Серый текст
    lv_obj_set_style_text_font( section_label, VW_FONT_14, LV_PART_MAIN );
    lv_obj_set_style_pad_top( section_label, 15, LV_PART_MAIN );
    lv_obj_set_style_pad_bottom( section_label, 5, LV_PART_MAIN );
    lv_obj_align( section_label, LV_ALIGN_LEFT_MID, 10, 0 );

    // 5. Список сетей
#if WINDOWS
    // ДЕМО ДАННЫЕ ДЛЯ ЭМУЛЯТОРА НА ПК 
    create_wifi_list_item( wifi_window, "Home_Sweet_Home", true, -45 );
    create_wifi_list_item( wifi_window, "Starbucks_Free", false, -70 );
    create_wifi_list_item( wifi_window, "Neighbors_5G", true, -85 );
    create_wifi_list_item( wifi_window, "Office_Guest", false, -90 );
#elif ESP32
    // Внимание: wifi_get_ap_info у вас блокирующая, интерфейс зависнет на время сканирования!
    wifi_ap_record_t ap_info[10];
    int ap_count = wifi_get_ap_info( 10, ap_info );

    if (ap_count > 0)
    {
        for (int i = 0; i < ap_count; i++)
        {
            bool is_secure = (ap_info[i].authmode != WIFI_AUTH_OPEN);
            create_wifi_list_item( wifi_window, (const char*)ap_info[i].ssid, is_secure, ap_info[i].rssi );
        }
    }
    else
    {
        // Если сетей не найдено
        lv_obj_t* no_wifi_label = lv_label_create( wifi_window );
        lv_label_set_text( no_wifi_label, "No networks found" );
        lv_obj_set_style_text_color( no_wifi_label, lv_color_hex( VW_GREY_COLOR_HEX ), LV_PART_MAIN );
    }
#endif

    // Анимация появления экрана (слайд снизу вверх)
    lv_anim_t a;
    lv_anim_init( &a );
    lv_anim_set_var( &a, wifi_window );
    lv_anim_set_time( &a, 300 );
    lv_anim_set_exec_cb( &a, (lv_anim_exec_xcb_t)lv_obj_set_y );
    lv_anim_set_values( &a, VW_LCD_V_RES, 0 ); // Выезжает снизу
    lv_anim_set_path_cb( &a, lv_anim_path_ease_out );
    lv_anim_start( &a );
}

//-----------------------------------------------------------------------------------------

#endif // MOD_LVGL_LCD && VW_WORK_MODE
