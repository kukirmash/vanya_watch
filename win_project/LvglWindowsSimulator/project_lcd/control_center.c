#include "control_center.h"

#if MOD_LVGL_LCD && VW_WORK_MODE

static const char* TAG = "VW_CONTROL_CENTER";

//-----------------------------------------------------------------------------------------
// Cоздание круглых кнопок
static lv_obj_t* grid_button_create( lv_obj_t* parent, int32_t col_pos, int32_t row_pos, const char* symbol )
{
    // Используем стандартную кнопку вместо switch
    lv_obj_t* obj = lv_button_create( parent );
    lv_obj_set_grid_cell( obj, LV_GRID_ALIGN_STRETCH, col_pos, 1, LV_GRID_ALIGN_STRETCH, row_pos, 1 );

    lv_obj_set_style_radius( obj, 40, LV_PART_MAIN );
    lv_obj_set_style_text_color( obj, lv_color_white(), LV_PART_MAIN );
    lv_obj_set_style_text_color( obj, lv_color_white(), LV_PART_MAIN | LV_STATE_CHECKED );

    lv_obj_t* label = lv_label_create( obj );
    lv_label_set_text( label, symbol );
    lv_obj_center( label );

    lv_obj_set_style_bg_opa( obj, 0, LV_PART_KNOB );
    lv_obj_set_style_text_font( label, VW_FONT_22, LV_PART_MAIN );

    return obj;
}

//-----------------------------------------------------------------------------------------
// Создание ползунка
static lv_obj_t* grid_slider_create( lv_obj_t* parent, int32_t col_pos, int32_t row_pos, const char* symbol )
{
    lv_obj_t* obj = lv_slider_create( parent );
    lv_obj_set_grid_cell( obj, LV_GRID_ALIGN_STRETCH, col_pos, 1, LV_GRID_ALIGN_STRETCH, row_pos, 2 );

    lv_obj_set_style_bg_opa( obj, 0, LV_PART_KNOB );
    lv_obj_set_style_radius( obj, 40, LV_PART_MAIN );
    lv_obj_set_style_radius( obj, 40, LV_PART_INDICATOR );

    lv_obj_t* label = lv_label_create( obj );
    lv_label_set_text( label, symbol );
    lv_obj_align( label, LV_ALIGN_BOTTOM_MID, 0, -20 );

    lv_obj_set_style_text_font( label, VW_FONT_22, LV_PART_MAIN );
    lv_obj_set_style_text_color( label, lv_color_white(), LV_PART_MAIN );

    return obj;
}

//-----------------------------------------------------------------------------------------
void control_center_init( lv_obj_t* parent )
{
    static int32_t col_dsc[] = { 65, 65, 65, LV_GRID_TEMPLATE_LAST };
    static int32_t row_dsc[] = { 65, 65, 65, LV_GRID_TEMPLATE_LAST };

    // tile - grid сетка
    lv_obj_set_layout( parent, LV_LAYOUT_GRID );
    lv_obj_set_grid_dsc_array( parent, col_dsc, row_dsc );

    lv_obj_set_style_pad_hor( parent, 11, LV_PART_MAIN );
    lv_obj_set_style_pad_column( parent, 11, LV_PART_MAIN );
    lv_obj_set_style_pad_row( parent, 11, LV_PART_MAIN );
    lv_obj_set_style_pad_ver( parent, 20, LV_PART_MAIN );

    // switch кнопки и slider
    lv_obj_t* wifi_btn = grid_button_create( parent, 0, 0, LV_SYMBOL_WIFI );
    lv_obj_add_flag( wifi_btn, LV_OBJ_FLAG_CHECKABLE );// Делаем кнопку "залипающей" (тумблер)

    lv_obj_t* bluetooth_btn = grid_button_create( parent, 1, 0, LV_SYMBOL_BLUETOOTH );
    lv_obj_add_flag( bluetooth_btn, LV_OBJ_FLAG_CHECKABLE );// Делаем кнопку "залипающей" (тумблер)

    lv_obj_t* battery_btn = grid_button_create( parent, 2, 0, LV_SYMBOL_BATTERY_2 );
    lv_obj_add_flag( battery_btn, LV_OBJ_FLAG_CHECKABLE );// Делаем кнопку "залипающей" (тумблер)

    lv_obj_t* settings_btn = grid_button_create( parent, 0, 1, LV_SYMBOL_SETTINGS );

    lv_obj_t* power_btn = grid_button_create( parent, 0, 2, LV_SYMBOL_POWER );

    lv_obj_t* volume_slider = grid_slider_create( parent, 1, 1, LV_SYMBOL_VOLUME_MID );

    lv_obj_t* brightness_slider = grid_slider_create( parent, 2, 1, VW_SYMBOL_BIRGHTNESS );
}

//-----------------------------------------------------------------------------------------

#endif // MOD_LVGL_LCD && VW_WORK_MODE
