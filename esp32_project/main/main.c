#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lvgl_port.h"
#include "esp_lcd_touch_cst816s.h"

#include "project_lcd/project_lcd_main.h"

//-----------------------------------------------------------------------------------------
/* LCD size (Standard for 1.69" LCD) */
#define VW_LCD_H_RES   (240)
#define VW_LCD_V_RES   (280)

/* LCD settings */
#define VW_LCD_SPI_NUM         (SPI2_HOST)
#define VW_LCD_PIXEL_CLK_HZ    (40 * 1000 * 1000)
#define VW_LCD_CMD_BITS        (8)
#define VW_LCD_PARAM_BITS      (8)
#define VW_LCD_BITS_PER_PIXEL  (16)
#define VW_LCD_DRAW_BUFF_DOUBLE (1)
#define VW_LCD_DRAW_BUFF_HEIGHT (50)
#define VW_LCD_BL_ON_LEVEL     (1)

/* LCD pins (From your config) */
#define VW_LCD_GPIO_SCLK       (GPIO_NUM_6)
#define VW_LCD_GPIO_MOSI       (GPIO_NUM_7)
#define VW_LCD_GPIO_RST        (GPIO_NUM_8)
#define VW_LCD_GPIO_DC         (GPIO_NUM_4)
#define VW_LCD_GPIO_CS         (GPIO_NUM_5)
#define VW_LCD_GPIO_BL         (GPIO_NUM_15)

/* Touch settings */
#define VW_TOUCH_I2C_NUM       (0)
#define VW_TOUCH_I2C_CLK_HZ    (400000)

/* LCD touch pins (From your config) */
#define VW_TOUCH_I2C_SCL       (GPIO_NUM_10)
#define VW_TOUCH_I2C_SDA       (GPIO_NUM_11)
#define VW_TOUCH_GPIO_RST      (GPIO_NUM_13)
#define VW_TOUCH_GPIO_INT      (GPIO_NUM_14)

//-----------------------------------------------------------------------------------------
static const char *TAG = "VANYAWATCH";

/* LCD IO and panel */
static esp_lcd_panel_io_handle_t lcd_io = NULL;
static esp_lcd_panel_handle_t lcd_panel = NULL;
static esp_lcd_touch_handle_t touch_handle = NULL;

/* LVGL display and touch */
static lv_display_t *lvgl_disp = NULL;
static lv_indev_t *lvgl_touch_indev = NULL;

//-----------------------------------------------------------------------------------------
static esp_err_t app_lcd_init(void)
{
    esp_err_t ret = ESP_OK;

    /* LCD backlight */
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << VW_LCD_GPIO_BL
    };
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));

    /* LCD initialization */
    ESP_LOGD(TAG, "Initialize SPI bus");
    const spi_bus_config_t buscfg = {
        .sclk_io_num = VW_LCD_GPIO_SCLK,
        .mosi_io_num = VW_LCD_GPIO_MOSI,
        .miso_io_num = GPIO_NUM_NC,
        .quadwp_io_num = GPIO_NUM_NC,
        .quadhd_io_num = GPIO_NUM_NC,
        .max_transfer_sz = VW_LCD_H_RES * VW_LCD_DRAW_BUFF_HEIGHT * sizeof(uint16_t),
    };
    ESP_RETURN_ON_ERROR(spi_bus_initialize(VW_LCD_SPI_NUM, &buscfg, SPI_DMA_CH_AUTO), TAG, "SPI init failed");

    ESP_LOGD(TAG, "Install panel IO");
    const esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = VW_LCD_GPIO_DC,
        .cs_gpio_num = VW_LCD_GPIO_CS,
        .pclk_hz = VW_LCD_PIXEL_CLK_HZ,
        .lcd_cmd_bits = VW_LCD_CMD_BITS,
        .lcd_param_bits = VW_LCD_PARAM_BITS,
        .spi_mode = 0,
        .trans_queue_depth = 10,
    };
    ESP_GOTO_ON_ERROR(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)VW_LCD_SPI_NUM, &io_config, &lcd_io), err, TAG, "New panel IO failed");

    ESP_LOGD(TAG, "Install LCD driver (ST7789)");

    const esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = VW_LCD_GPIO_RST,
        .rgb_endian = LCD_RGB_ENDIAN_RGB,
        .bits_per_pixel = VW_LCD_BITS_PER_PIXEL,
    };

    ESP_GOTO_ON_ERROR(esp_lcd_new_panel_st7789(lcd_io, &panel_config, &lcd_panel), err, TAG, "New panel failed");

    esp_lcd_panel_reset(lcd_panel);
    esp_lcd_panel_init(lcd_panel);
    
    esp_lcd_panel_set_gap(lcd_panel, 0, 20);
    esp_lcd_panel_invert_color(lcd_panel, true);
    esp_lcd_panel_mirror(lcd_panel, true, true);
    
    esp_lcd_panel_disp_on_off(lcd_panel, true);

    /* LCD backlight on */
    ESP_ERROR_CHECK(gpio_set_level(VW_LCD_GPIO_BL, VW_LCD_BL_ON_LEVEL));

    return ret;

err:
    if (lcd_panel) 
        esp_lcd_panel_del(lcd_panel);
    if (lcd_io) 
        esp_lcd_panel_io_del(lcd_io);

    spi_bus_free(VW_LCD_SPI_NUM);

    return ret;
}

//-----------------------------------------------------------------------------------------
static esp_err_t app_touch_init(void)
{
    /* Initialize I2C (Современный API) */
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

    /* Initialize touch HW (CST816S) */
    const esp_lcd_touch_config_t tp_cfg = {
        .x_max = VW_LCD_H_RES,
        .y_max = VW_LCD_V_RES,
        .rst_gpio_num = VW_TOUCH_GPIO_RST,
        .int_gpio_num = VW_TOUCH_GPIO_INT,
        .levels = {
            .reset = 0,
            .interrupt = 0,
        },
        .flags = {
            .swap_xy = 0,
            .mirror_x = 0,
            .mirror_y = 0,
        },
    };

    esp_lcd_panel_io_handle_t tp_io_handle = NULL;
    esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_CST816S_CONFIG();
    tp_io_config.scl_speed_hz = VW_TOUCH_I2C_CLK_HZ;
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_i2c(i2c_handle, &tp_io_config, &tp_io_handle), TAG, "");
    
    return esp_lcd_touch_new_i2c_cst816s(tp_io_handle, &tp_cfg, &touch_handle);
}

//-----------------------------------------------------------------------------------------
static esp_err_t app_lvgl_init(void)
{
    /* Initialize LVGL */
    const lvgl_port_cfg_t lvgl_cfg = {
        .task_priority = 4,
        .task_stack = 8192,         /* Увеличено для надежности */
        .task_affinity = -1,
        .task_max_sleep_ms = 500,
        .timer_period_ms = 5
    };
    ESP_RETURN_ON_ERROR(lvgl_port_init(&lvgl_cfg), TAG, "LVGL port initialization failed");

    /* Add LCD screen */
    ESP_LOGD(TAG, "Add LCD screen");
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
        .rotation = {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false,
        },
        .flags = {
            .buff_dma = true,
#if LVGL_VERSION_MAJOR >= 9
            .swap_bytes = true,
#endif
        }
    };
    lvgl_disp = lvgl_port_add_disp(&disp_cfg);

    /* Add touch input */
    const lvgl_port_touch_cfg_t touch_cfg = {
        .disp = lvgl_disp,
        .handle = touch_handle,
    };
    lvgl_touch_indev = lvgl_port_add_touch(&touch_cfg);

    return ESP_OK;
}

//-----------------------------------------------------------------------------------------
static void app_main_display(void)
{
    /* Task lock */
    lvgl_port_lock(0);

    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x000000), 0); // Черный фон

    lvgl_lcd();

    /* Task unlock */
    lvgl_port_unlock();
}

//-----------------------------------------------------------------------------------------
void app_main(void)
{
    /* LCD HW initialization */
    ESP_ERROR_CHECK(app_lcd_init());

    /* Touch initialization */
    ESP_ERROR_CHECK(app_touch_init());

    /* LVGL initialization */
    ESP_ERROR_CHECK(app_lvgl_init());

    /* Show LVGL objects */
    app_main_display();
}

//-----------------------------------------------------------------------------------------