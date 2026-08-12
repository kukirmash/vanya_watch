#include "app_ui/app_ui_main.h"

#include "lvgl/lvgl.h"
#include "config/ui_config.h"
#include "app_ui/windows/watchface.h"

static const char *TAG = "PROJECT_LCD_MAIN";

// Внутренние переменные LVGL
static lv_display_t *lvgl_disp = NULL;
static lv_indev_t *lvgl_touch_indev = NULL;

#if ESP32

#include "esp_log.h"
#include "esp_check.h"
#include "esp_lvgl_port.h"

//-----------------------------------------------------------------------------------------
esp_err_t app_lvgl_init(esp_lcd_panel_io_handle_t lcd_io, esp_lcd_panel_handle_t lcd_panel, esp_lcd_touch_handle_t touch_handle)
{
	const lvgl_port_cfg_t lvgl_cfg = {
		.task_priority = 4,
		.task_stack = 8192,
		.task_affinity = -1,
		.task_max_sleep_ms = 500,
		.timer_period_ms = 2};
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
		.rotation = {.swap_xy = false, .mirror_x = false, .mirror_y = false},
		.flags = {
			.buff_dma = true,
#if LVGL_VERSION_MAJOR >= 9
			.swap_bytes = true,
#endif
		}};
	lvgl_disp = lvgl_port_add_disp(&disp_cfg);

	const lvgl_port_touch_cfg_t touch_cfg = {
		.disp = lvgl_disp,
		.handle = touch_handle,
	};
	lvgl_touch_indev = lvgl_port_add_touch(&touch_cfg);

	return ESP_OK;
}

//-----------------------------------------------------------------------------------------
#endif // ESP32

//-----------------------------------------------------------------------------------------
void app_ui_main()
{
	lv_lock();

	// Темная тема
	lv_theme_t *dark_theme = lv_theme_default_init(lvgl_disp, lv_color_hex(VW_PRIMARY_COLOR_HEX), lv_color_hex(VW_SECONDARY_COLOR_HEX), true, VW_FONT_14);
	lv_display_set_theme(lvgl_disp, dark_theme);

	lv_obj_t *screen = lv_screen_active();
	lv_obj_set_style_bg_color(screen, lv_color_hex(0x000000), 0);

	watchface_init(screen);

	lv_unlock();
}

//-----------------------------------------------------------------------------------------