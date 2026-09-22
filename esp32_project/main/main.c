#include "board/power.h"
#include "board/lcd.h"
#include "board/touch.h"
#include "board/time.h"
#include "board/wifi.h"

#include "app_ui/app_ui_main.h"

//-----------------------------------------------------------------------------------------
void app_main(void)
{
	// 1. Питание: перехват питания (hold) и АЦП батареи
	power_init();

	// 2. Инициализация дисплея и тача
	esp_lcd_panel_io_handle_t lcd_io = NULL;
	esp_lcd_panel_handle_t lcd_panel = NULL;
	esp_lcd_touch_handle_t touch_handle = NULL;
	ESP_ERROR_CHECK(lcd_app_init(&lcd_io, &lcd_panel));
	ESP_ERROR_CHECK(app_touch_init(&touch_handle));

	// 3. Порт LVGL (создаёт мьютексы) - ДОЛЖЕН быть до любых обращений к LVGL
	ESP_ERROR_CHECK(app_lvgl_init(lcd_io, lcd_panel, touch_handle));

	// 4. Фоновые сервисы (время и Wi-Fi). Теперь мьютекс LVGL уже доступен.
	board_time_init();
	board_wifi_init();

	// 5. Построение интерфейса (менеджер окон + окна)
	app_ui_main();
}

//-----------------------------------------------------------------------------------------
