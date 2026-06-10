#include "project_lcd_main.h"

#if MOD_LVGL_LCD

#include "../board/lcd.h"// TODO: VW_LCD_H_RES вынести в project_config
#include "esp_log.h"
#include "esp_check.h"
#include "esp_lvgl_port.h"

#include "watchface.h"
#include "control_center.h"
#include "demo_sequential.h"

static const char* TAG = "VW_PROJECT_LCD_MAIN";

// Внутренние переменные LVGL
static lv_display_t *lvgl_disp = NULL;
static lv_indev_t *lvgl_touch_indev = NULL;

//-----------------------------------------------------------------------------------------
esp_err_t app_lvgl_init(esp_lcd_panel_io_handle_t lcd_io, esp_lcd_panel_handle_t lcd_panel, esp_lcd_touch_handle_t touch_handle)
{
    const lvgl_port_cfg_t lvgl_cfg = {
        .task_priority = 4,
        .task_stack = 8192,
        .task_affinity = -1,
        .task_max_sleep_ms = 500,
        .timer_period_ms = 2
    };
    ESP_RETURN_ON_ERROR(lvgl_port_init(&lvgl_cfg), TAG, "LVGL port init failed");

    ESP_LOGI(TAG, "Add LCD screen to LVGL");
    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = lcd_io,
        .panel_handle = lcd_panel,
        .buffer_size = VW_LCD_H_RES * VW_LCD_DRAW_BUFF_HEIGHT,
        .double_buffer = VW_LCD_DRAW_BUFF_DOUBLE,
        .hres = VW_LCD_H_RES,
        .vres = VW_LCD_V_RES,
        .monochrome = false,
#if LVGL_VERSION_MAJOR >= 9
        .color_format = LV_COLOR_FORMAT_RGB565,
#endif
        .rotation = { .swap_xy = false, .mirror_x = false, .mirror_y = false },
        .flags = {
            .buff_dma = true,
#if LVGL_VERSION_MAJOR >= 9
            .swap_bytes = true,
#endif
        }
    };
    lvgl_disp = lvgl_port_add_disp(&disp_cfg);

    const lvgl_port_touch_cfg_t touch_cfg = {
        .disp = lvgl_disp,
        .handle = touch_handle,
    };
    lvgl_touch_indev = lvgl_port_add_touch(&touch_cfg);

    return ESP_OK;
}

//-----------------------------------------------------------------------------------------
void app_main_display(void)
{
    lvgl_port_lock(0);
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x000000), 0);
    
    lvgl_lcd();
    
    lvgl_port_unlock();
}


//-----------------------------------------------------------------------------------------
void lvgl_lcd()
{
    // Темная тема
    lv_display_set_theme( lvgl_disp, lv_theme_default_init( lvgl_disp, lv_color_hex( VW_PRIMARY_COLOR_HEX), lv_color_hex( VW_SECONDARY_COLOR_HEX), true, VW_FONT_14 ) );

    lv_obj_t* scr = lv_screen_active();

#if VW_DEMO_SEQUENTIAL_ANIMATION
    demo_sequential_init(); // Запускаем демо
#endif // VW_DEMO_SEQUENTIAL_ANIMATION

#if VW_WORK_MODE

    lv_obj_t* tileview = lv_tileview_create( scr );
    lv_obj_set_size( tileview, lv_pct( 100 ), lv_pct( 100 ) );
    lv_obj_set_style_bg_color( tileview, lv_color_hex( 0x000000 ), LV_PART_MAIN ); // Черный фон
    lv_obj_set_style_radius( tileview, 50, LV_PART_MAIN );
    lv_obj_set_style_pad_all( tileview, 0, LV_PART_MAIN );
    lv_obj_set_scrollbar_mode( tileview, LV_SCROLLBAR_MODE_OFF );
    lv_obj_remove_flag(tileview, LV_OBJ_FLAG_SCROLL_ELASTIC);

    lv_obj_t* main_tile = lv_tileview_add_tile( tileview, 1, 1, LV_DIR_ALL );
    lv_obj_t* control_center_tile = lv_tileview_add_tile( tileview, 1, 0, LV_DIR_ALL );

    watchface_init( main_tile );       
    control_center_init( control_center_tile );

    lv_tileview_set_tile( tileview, main_tile, LV_ANIM_OFF );
#endif // VW_WORK_MODE
}

//-----------------------------------------------------------------------------------------

#endif // MOD_LVGL_LCD
