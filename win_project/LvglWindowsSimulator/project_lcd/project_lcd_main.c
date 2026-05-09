#include "project_lcd_main.h"

#if MOD_LVGL_LCD

#include "watchface.h"
#include "control_center.h"
#include "demo_sequential.h"

static const char* TAG = "VW_PROJECT_LCD_MAIN";

//-----------------------------------------------------------------------------------------
void lvgl_lcd()
{
    // Темная тема
    lv_display_t* display = lv_display_get_default();
    lv_display_set_theme( display, lv_theme_default_init( display, lv_color_hex( VW_PRIMARY_COLOR_HEX), lv_color_hex( VW_SECONDARY_COLOR_HEX), true, VW_FONT_14 ) );

    lv_obj_t* scr = lv_screen_active();

#if VW_DEMO_SEQUENTIAL_ANIMATION
    demo_sequential_init(); // Запускаем демо
#endif

#if VW_WORK_MODE

    lv_obj_t* tileview = lv_tileview_create( scr );
    lv_obj_set_size( tileview, lv_pct( 100 ), lv_pct( 100 ) );
    lv_obj_set_style_bg_color( tileview, lv_color_hex( 0x000000 ), LV_PART_MAIN ); // Черный фон
    lv_obj_set_style_radius( tileview, 50, LV_PART_MAIN );
    lv_obj_set_style_pad_all( tileview, 0, LV_PART_MAIN );
    lv_obj_set_scrollbar_mode( tileview, LV_SCROLLBAR_MODE_OFF );

    lv_obj_t* main_tile = lv_tileview_add_tile( tileview, 1, 1, LV_DIR_ALL );
    lv_obj_t* control_center_tile = lv_tileview_add_tile( tileview, 1, 0, LV_DIR_ALL );

    watchface_init( main_tile );       
    control_center_init( control_center_tile );

    lv_tileview_set_tile( tileview, main_tile, LV_ANIM_OFF );
#endif
}

//-----------------------------------------------------------------------------------------

#endif // MOD_LVGL_LCD
