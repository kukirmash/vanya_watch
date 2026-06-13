#include "wifi_window.h"

#if MOD_LVGL_LCD && VW_WORK_MODE

//-----------------------------------------------------------------------------------------
#include "../config/project_config.h"
#include "keyboard_window.h"

#if ESP32
#include "board/wifi.h"
#endif

static const char* TAG = "VW_WIFI_WINDOW";

static lv_obj_t* wifi_window = NULL;
static lv_obj_t* wifi_list_cont = NULL;
static lv_timer_t* scan_timer = NULL;
static bool is_scanning = false;

static char target_ssid[ 32 ] = { 0 };
static char temp_password[ 64 ] = { 0 };

//-----------------------------------------------------------------------------------------
static void back_btn_event_cb( lv_event_t* e )
{
	lv_event_code_t code = lv_event_get_code( e );
	if ( code == LV_EVENT_CLICKED && wifi_window != NULL )
	{
		if ( scan_timer )
		{
			lv_timer_delete( scan_timer );
			scan_timer = NULL;
		}
		lv_obj_delete_async( wifi_window );
		wifi_window = NULL;
		wifi_list_cont = NULL;
	}
}

//-----------------------------------------------------------------------------------------
// Функция, которая вызовется, когда мы нажмем Done на клавиатуре
static void on_wifi_password_done( const char* entered_password )
{
    LOGI( TAG, "Connecting to %s with password: %s", target_ssid, entered_password );
#if ESP32
    wifi_connect_to_ap( target_ssid, entered_password );
#endif
}

//-----------------------------------------------------------------------------------------
// Обработчик клика по конкретной сети в списке
static void wifi_list_item_event_cb( lv_event_t* e )
{
    lv_obj_t* btn = lv_event_get_target( e );

    lv_obj_t* ssid_label = lv_obj_get_child( btn, 0 );
    const char* ssid = lv_label_get_text( ssid_label );
    bool is_secure = ( lv_obj_get_child_count( btn ) == 3 );

    LOGI( TAG, "User clicked on network: %s", ssid );

    // Сохраняем имя сети в статический буфер (он дождется кнопки Done)
    strncpy( target_ssid, ssid, sizeof( target_ssid ) - 1 );
    target_ssid[ sizeof( target_ssid ) - 1 ] = '\0';

    if ( is_secure )
    {
        // Очищаем статический буфер пароля перед новым вводом
        memset( temp_password, 0, sizeof( temp_password ) );

        keyboard_window_create(
            target_ssid,               // Заголовок - имя сети
            temp_password,             // СТАТИЧЕСКИЙ БУФЕР (Не уничтожится!)
            sizeof( temp_password ),   // Размер
            KB_INPUT_PASSWORD,         // Тип - скрытый пароль
            on_wifi_password_done      // Что делать после "Done"
        );
    }
    else
    {
        // Если сеть без пароля - подключаемся сразу
        LOGI( TAG, "Network is open, connecting directly..." );
#if ESP32
        wifi_connect_to_ap( target_ssid, "" );
#endif
    }
}

//-----------------------------------------------------------------------------------------
static lv_obj_t* create_wifi_list_item( lv_obj_t* parent, const char* ssid, bool is_secure, int rssi )
{
	lv_obj_t* item = lv_button_create( parent );
	lv_obj_set_size( item, lv_pct( 100 ), LV_SIZE_CONTENT );
	lv_obj_set_layout( item, LV_LAYOUT_FLEX );
	lv_obj_set_flex_flow( item, LV_FLEX_FLOW_ROW );
	lv_obj_set_flex_align( item, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER );
	lv_obj_add_event_cb( item, wifi_list_item_event_cb, LV_EVENT_CLICKED, NULL );
	lv_obj_set_style_bg_color( item, lv_color_hex( VW_DARK_GREY_COLOR_HEX ), LV_PART_MAIN );
	lv_obj_set_style_radius( item, 16, LV_PART_MAIN );
	lv_obj_set_style_pad_all( item, 15, LV_PART_MAIN );

	lv_obj_t* ssid_label = lv_label_create( item );
	lv_label_set_text( ssid_label, ssid );
	lv_obj_set_style_text_color( ssid_label, lv_color_white(), LV_PART_MAIN );
	lv_obj_set_style_text_font( ssid_label, VW_FONT_18, LV_PART_MAIN );
	lv_obj_set_flex_grow( ssid_label, 1 );

	if ( is_secure )
	{
		lv_obj_t* lock_icon = lv_label_create( item );
		lv_label_set_text( lock_icon, VW_SYMBOL_LOCK );
		lv_obj_set_style_text_color( lock_icon, lv_color_hex( VW_GREY_COLOR_HEX ), LV_PART_MAIN );
		lv_obj_set_style_text_font( lock_icon, VW_FONT_14, LV_PART_MAIN );
	}

	const char* wifi_sym = LV_SYMBOL_WIFI;
	if ( rssi < -75 )
		wifi_sym = VW_SYMBOL_WIFI_LOW;
	else if ( rssi < -60 )
		wifi_sym = VW_SYMBOL_WIFI_MID;
	lv_obj_t* signal_icon = lv_label_create( item );
	lv_label_set_text( signal_icon, wifi_sym );
	lv_obj_set_style_text_color( signal_icon, lv_color_hex( VW_PRIMARY_COLOR_HEX ), LV_PART_MAIN );

	return item;
}

#if ESP32
//-----------------------------------------------------------------------------------------
static void wifi_scan_task( void* arg )
{
	wifi_ap_record_t ap_info[15];
	memset( ap_info, 0, sizeof( ap_info ) );

	int ap_count = wifi_get_ap_info( 15, ap_info );

	lvgl_port_lock( 0 );

	// Снова запрашиваем конфиг, чтобы узнать, не выключили ли Wi-Fi, пока шло сканирование
	pc_wifi_config wifi_cfg;
	project_config_get_wifi( &wifi_cfg );

	

	is_scanning = false;
	lvgl_port_unlock();

	vTaskDelete( NULL );
}

//-----------------------------------------------------------------------------------------
static void scan_timer_cb( lv_timer_t* timer )
{
	pc_wifi_config wifi_cfg;
	project_config_get_wifi( &wifi_cfg );

	if ( wifi_cfg.is_enabled && !is_scanning )
	{
		is_scanning = true;
		xTaskCreate( wifi_scan_task, "wifi_scan_task", 4096, NULL, 5, NULL );
	}
}

#elif WINDOWS

//-----------------------------------------------------------------------------------------
static void scan_timer_cb( lv_timer_t* timer )
{
    int ap_count = 3;

    pc_wifi_config wifi_cfg;
    project_config_get_wifi( &wifi_cfg );

    if (wifi_cfg.is_enabled)
    {

        if (wifi_window != NULL && wifi_list_cont != NULL)
        {
            // Если Wi-Fi включен - рисуем список
            if (wifi_cfg.is_enabled && ap_count >= 0)
            {
                lv_obj_clean( wifi_list_cont ); // очищаем полностью контейнер

                if (ap_count > 0)
                {
                    for (int i = 0; i < ap_count; i++)
                    {
                        bool is_secure = true;
                        create_wifi_list_item( wifi_list_cont, "test", is_secure, -65 );
                    }
                }
                else
                {
                    lv_obj_t* no_wifi_label = lv_label_create( wifi_list_cont );
                    lv_label_set_text( no_wifi_label, "No networks found" );
                    lv_obj_set_style_text_color( no_wifi_label, lv_color_hex( VW_GREY_COLOR_HEX ), LV_PART_MAIN );
                    lv_obj_set_width( no_wifi_label, lv_pct( 100 ) );
                    lv_obj_set_style_text_align( no_wifi_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN );
                }
            }
        }

    }
}

#endif

//-----------------------------------------------------------------------------------------
static void current_network_observer_cb( lv_observer_t* observer, lv_subject_t* subject )
{
	lv_obj_t* label = lv_observer_get_target( observer );
	const pc_wifi_config* wifi_data = lv_subject_get_pointer( subject );
	if ( !wifi_data )
		return;

	if ( wifi_data->is_connected && wifi_data->ssid[0] != '\0' )
	{
		lv_label_set_text( label, wifi_data->ssid );
		lv_obj_set_style_text_color( label, lv_color_hex( VW_PRIMARY_COLOR_HEX ), LV_PART_MAIN );
	}
	else if ( wifi_data->is_enabled )
	{
		lv_label_set_text( label, "Not connected" );
		lv_obj_set_style_text_color( label, lv_color_white(), LV_PART_MAIN );
	}
	else
	{
		lv_label_set_text( label, "Wi-Fi is off" );
		lv_obj_set_style_text_color( label, lv_color_hex( VW_GREY_COLOR_HEX ), LV_PART_MAIN );
	}
}

//-----------------------------------------------------------------------------------------
static void wifi_switch_event_cb( lv_event_t* e )
{
	lv_obj_t* sw = lv_event_get_target( e );
	bool is_on = lv_obj_has_state( sw, LV_STATE_CHECKED );

#if ESP32
	wifi_set_state( is_on );
#elif WINDOWS
    project_config_set_wifi_enabled( is_on );
#endif

    scan_timer_cb( NULL );
    if (!is_on && wifi_list_cont != NULL)
        lv_obj_clean( wifi_list_cont );
}

//-----------------------------------------------------------------------------------------
void wifi_window_create( void )
{
	if ( wifi_window != NULL )
		return;

	lv_obj_t* parent = lv_screen_active();

	// Главное окно 
	wifi_window = lv_obj_create( parent );
	lv_obj_set_size( wifi_window, lv_pct( 100 ), lv_pct( 100 ) );
	lv_obj_set_layout( wifi_window, LV_LAYOUT_FLEX );
	lv_obj_set_flex_flow( wifi_window, LV_FLEX_FLOW_COLUMN );
	lv_obj_set_scrollbar_mode( wifi_window, LV_SCROLLBAR_MODE_OFF );
	lv_obj_set_style_bg_color( wifi_window, lv_color_black(), LV_PART_MAIN );
	lv_obj_set_style_border_width( wifi_window, 0, LV_PART_MAIN );
	lv_obj_set_style_radius( wifi_window, 0, LV_PART_MAIN );
	lv_obj_set_style_pad_all( wifi_window, 0, LV_PART_MAIN );

	// 1. Шапка 
	lv_obj_t* header = lv_obj_create( wifi_window );
	lv_obj_set_size( header, lv_pct( 100 ), 30 );
	lv_obj_remove_flag( header, LV_OBJ_FLAG_SCROLLABLE );
	lv_obj_set_style_bg_opa( header, 0, LV_PART_MAIN );
	lv_obj_set_style_border_width( header, 0, LV_PART_MAIN );
	lv_obj_set_style_pad_hor( header, 15, LV_PART_MAIN );
	lv_obj_set_style_pad_ver( header, 0, LV_PART_MAIN );

	// Кнопка "Назад"
	lv_obj_t* back_btn = lv_button_create( header );
	lv_obj_align( back_btn, LV_ALIGN_BOTTOM_LEFT, 0, 0 );
	lv_obj_add_event_cb( back_btn, back_btn_event_cb, LV_EVENT_CLICKED, NULL );
	lv_obj_set_style_bg_opa( back_btn, 0, LV_PART_MAIN );
	lv_obj_set_style_shadow_width( back_btn, 0, LV_PART_MAIN );
	lv_obj_set_style_pad_all( back_btn, 0, LV_PART_MAIN );

	// "< Wi-Fi" в кнопке
	lv_obj_t* back_label = lv_label_create( back_btn );
	lv_obj_align( back_label, LV_ALIGN_BOTTOM_MID, 0, 0 );
	lv_label_set_text( back_label, LV_SYMBOL_LEFT " Wi-Fi" );
	lv_obj_set_style_text_color( back_label, lv_color_hex( VW_PRIMARY_COLOR_HEX ), LV_PART_MAIN );
	lv_obj_set_style_text_font( back_label, VW_FONT_18, LV_PART_MAIN );

	// Время слева
	lv_obj_t* time_label = lv_label_create( header );
	lv_obj_align( time_label, LV_ALIGN_BOTTOM_RIGHT, 0, 0 );
	lv_label_bind_text( time_label, &subject_time_str, NULL );
	lv_obj_set_style_text_color( time_label, lv_color_white(), LV_PART_MAIN );
	lv_obj_set_style_text_font( time_label, VW_FONT_18, LV_PART_MAIN );

	// КОНТЕЙНЕР ДЛЯ СКРОЛЛА
	lv_obj_t* scroll_cont = lv_obj_create( wifi_window );
	lv_obj_set_width( scroll_cont, lv_pct( 100 ) );
	lv_obj_set_layout( scroll_cont, LV_LAYOUT_FLEX );
	lv_obj_set_flex_flow( scroll_cont, LV_FLEX_FLOW_COLUMN );
	lv_obj_set_flex_grow( scroll_cont, 1 );
	lv_obj_set_scrollbar_mode( scroll_cont, LV_SCROLLBAR_MODE_AUTO );
	lv_obj_remove_flag( scroll_cont, LV_OBJ_FLAG_SCROLL_ELASTIC );
	lv_obj_set_style_bg_opa( scroll_cont, 0, LV_PART_MAIN );
	lv_obj_set_style_border_width( scroll_cont, 0, LV_PART_MAIN );
	lv_obj_set_style_pad_ver( scroll_cont, 0, LV_PART_MAIN );
	lv_obj_set_style_pad_hor( scroll_cont, 10, LV_PART_MAIN );
	lv_obj_set_style_pad_bottom( scroll_cont, 10, LV_PART_MAIN );
	lv_obj_set_style_bg_color( scroll_cont, lv_color_hex( VW_PRIMARY_COLOR_HEX ), LV_PART_SCROLLBAR );

	// 2. Единый блок: Свитчер + Линия + Статус
	lv_obj_t* main_toggle_cont = lv_obj_create( scroll_cont );
	lv_obj_set_size( main_toggle_cont, lv_pct( 100 ), LV_SIZE_CONTENT );
	lv_obj_set_layout( main_toggle_cont, LV_LAYOUT_FLEX );
	lv_obj_set_flex_flow( main_toggle_cont, LV_FLEX_FLOW_COLUMN );
	lv_obj_set_style_bg_color( main_toggle_cont, lv_color_hex( VW_DARK_GREY_COLOR_HEX ), LV_PART_MAIN );
	lv_obj_set_style_border_width( main_toggle_cont, 0, LV_PART_MAIN );
	lv_obj_set_style_radius( main_toggle_cont, 16, LV_PART_MAIN );
	lv_obj_set_style_pad_all( main_toggle_cont, 15, LV_PART_MAIN );
	lv_obj_set_style_pad_gap( main_toggle_cont, 10, LV_PART_MAIN );

	// Верхняя строка: текст Wi-Fi и Свитчер
	lv_obj_t* top_row = lv_obj_create( main_toggle_cont );
	lv_obj_set_size( top_row, lv_pct( 100 ), LV_SIZE_CONTENT );
	lv_obj_set_layout( top_row, LV_LAYOUT_FLEX );
	lv_obj_set_flex_flow( top_row, LV_FLEX_FLOW_ROW );
	lv_obj_set_flex_align( top_row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER );
	lv_obj_set_style_bg_opa( top_row, 0, LV_PART_MAIN );
	lv_obj_set_style_border_width( top_row, 0, LV_PART_MAIN );
	lv_obj_set_style_pad_all( top_row, 0, LV_PART_MAIN );

	lv_obj_t* toggle_label = lv_label_create( top_row );
	lv_label_set_text( toggle_label, "Wi-Fi" );
	lv_obj_set_style_text_color( toggle_label, lv_color_white(), LV_PART_MAIN );
	lv_obj_set_style_text_font( toggle_label, VW_FONT_18, LV_PART_MAIN );

	pc_wifi_config curr_wifi_cfg;
	project_config_get_wifi( &curr_wifi_cfg );
	lv_obj_t* sw = lv_switch_create( top_row );
	lv_obj_set_state( sw, LV_STATE_CHECKED, curr_wifi_cfg.is_enabled );
	lv_obj_add_event_cb( sw, wifi_switch_event_cb, LV_EVENT_VALUE_CHANGED, NULL );
	lv_obj_set_style_bg_color( sw, lv_color_hex( VW_PRIMARY_COLOR_HEX ), LV_PART_INDICATOR | LV_STATE_CHECKED );

	//  Линия разделитель
	lv_obj_t* divider = lv_obj_create( main_toggle_cont );
	lv_obj_set_size( divider, lv_pct( 100 ), 1 );
	lv_obj_set_style_bg_color( divider, lv_color_hex( VW_GREY_COLOR_HEX ), LV_PART_MAIN );
	lv_obj_set_style_border_width( divider, 0, LV_PART_MAIN );
	lv_obj_set_style_pad_all( divider, 0, LV_PART_MAIN );

	// Нижняя строка: Статус по левому краю
	lv_obj_t* status_label = lv_label_create( main_toggle_cont );
	lv_obj_set_width( status_label, lv_pct( 100 ) );
	lv_subject_add_observer_obj( &subject_wifi, current_network_observer_cb, status_label, NULL );
	lv_obj_set_style_text_font( status_label, VW_FONT_18, LV_PART_MAIN );

	// 3. Заголовок CHOOSE NETWORK
	lv_obj_t* section_label = lv_label_create( scroll_cont );
	lv_obj_set_width( section_label, lv_pct( 100 ) );
	lv_label_set_text( section_label, "CHOOSE NETWORK" );
	lv_obj_set_style_text_color( section_label, lv_color_hex( VW_GREY_COLOR_HEX ), LV_PART_MAIN );
	lv_obj_set_style_text_font( section_label, VW_FONT_14, LV_PART_MAIN );
	lv_obj_set_style_text_align( section_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN );
	lv_obj_set_style_pad_top( section_label, 15, LV_PART_MAIN );
	lv_obj_set_style_pad_bottom( section_label, 5, LV_PART_MAIN );

	// 4. Контейнер для списка
	wifi_list_cont = lv_obj_create( scroll_cont );
	lv_obj_set_size( wifi_list_cont, lv_pct( 100 ), LV_SIZE_CONTENT );
	lv_obj_set_layout( wifi_list_cont, LV_LAYOUT_FLEX );
	lv_obj_set_flex_flow( wifi_list_cont, LV_FLEX_FLOW_COLUMN );
	lv_obj_set_style_bg_opa( wifi_list_cont, 0, LV_PART_MAIN );
	lv_obj_set_style_border_width( wifi_list_cont, 0, LV_PART_MAIN );
	lv_obj_set_style_pad_ver( wifi_list_cont, 5, LV_PART_MAIN );
	lv_obj_set_style_pad_hor( wifi_list_cont, 0, LV_PART_MAIN );

	// Анимация появления
	lv_anim_t a;
	lv_anim_init( &a );
	lv_anim_set_var( &a, wifi_window );
	lv_anim_set_time( &a, 300 );
	lv_anim_set_exec_cb( &a, ( lv_anim_exec_xcb_t )lv_obj_set_y );
	lv_anim_set_values( &a, VW_LCD_V_RES, 0 );
	lv_anim_set_path_cb( &a, lv_anim_path_ease_out );
	lv_anim_start( &a );

	// Запускаем таймер сканирования
	scan_timer = lv_timer_create( scan_timer_cb, 10000, NULL );
	lv_timer_ready( scan_timer );
}

#endif // MOD_LVGL_LCD && VW_WORK_MODE
