#include "touch.h"
#include "lcd.h" // TODO: VW_LCD_H_RES вынести в project_config
#include "esp_log.h"
#include "esp_check.h"
#include "driver/i2c_master.h"

static const char *TAG = "TOUCH";

//-----------------------------------------------------------------------------------------
/* Touch settings */
#define VW_TOUCH_I2C_NUM (0)
#define VW_TOUCH_I2C_CLK_HZ (400000)

/* LCD touch pins */
#define VW_TOUCH_I2C_SCL (GPIO_NUM_10)
#define VW_TOUCH_I2C_SDA (GPIO_NUM_11)
#define VW_TOUCH_GPIO_RST (GPIO_NUM_13)
#define VW_TOUCH_GPIO_INT (GPIO_NUM_14)

//-----------------------------------------------------------------------------------------
esp_err_t app_touch_init(esp_lcd_touch_handle_t *ret_touch)
{
    esp_lcd_touch_handle_t touch_handle = NULL;
    i2c_master_bus_handle_t i2c_handle = NULL;

    const i2c_master_bus_config_t i2c_config = {
        .i2c_port = VW_TOUCH_I2C_NUM,
        .sda_io_num = VW_TOUCH_I2C_SDA,
        .scl_io_num = VW_TOUCH_I2C_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_RETURN_ON_ERROR(i2c_new_master_bus(&i2c_config, &i2c_handle), TAG, "I2C init failed");

    const esp_lcd_touch_config_t tp_cfg = {
        .x_max = VW_LCD_H_RES,
        .y_max = VW_LCD_V_RES,
        .rst_gpio_num = VW_TOUCH_GPIO_RST,
        .int_gpio_num = VW_TOUCH_GPIO_INT,
        .levels = { .reset = 0, .interrupt = 0 },
        .flags = { .swap_xy = 0, .mirror_x = 0, .mirror_y = 0 },
    };

    esp_lcd_panel_io_handle_t tp_io_handle = NULL;
    esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_CST816S_CONFIG();
    tp_io_config.scl_speed_hz = VW_TOUCH_I2C_CLK_HZ;
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_i2c(i2c_handle, &tp_io_config, &tp_io_handle), TAG, "");

    esp_err_t ret = esp_lcd_touch_new_i2c_cst816s(tp_io_handle, &tp_cfg, &touch_handle);
    
    *ret_touch = touch_handle;
    return ret;
}

//-----------------------------------------------------------------------------------------
