#include "lcd.h"

#include "esp_log.h"
#include "esp_check.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_lcd_panel_vendor.h"

#include "config/project_config.h"

static const char* TAG = "LCD";

//-----------------------------------------------------------------------------------------
/* LCD settings */
#define VW_LCD_SPI_NUM (SPI2_HOST)
#define VW_LCD_PIXEL_CLK_HZ (80 * 1000 * 1000)
#define VW_LCD_CMD_BITS (8)
#define VW_LCD_PARAM_BITS (8)
#define VW_LCD_BITS_PER_PIXEL (16)
#define VW_LCD_BL_ON_LEVEL (1)

/* LCD pins */
#define VW_LCD_GPIO_SCLK (GPIO_NUM_6)
#define VW_LCD_GPIO_MOSI (GPIO_NUM_7)
#define VW_LCD_GPIO_RST (GPIO_NUM_8)
#define VW_LCD_GPIO_DC (GPIO_NUM_4)
#define VW_LCD_GPIO_CS (GPIO_NUM_5)
#define VW_LCD_GPIO_BL (GPIO_NUM_15)

static esp_lcd_panel_handle_t lcd_panel = NULL;

//-----------------------------------------------------------------------------------------
esp_err_t lcd_app_init( esp_lcd_panel_io_handle_t* ret_io, esp_lcd_panel_handle_t* ret_panel )
{
	esp_err_t ret = ESP_OK;
	esp_lcd_panel_io_handle_t lcd_io = NULL;

	/* LCD backlight */
	gpio_config_t bk_gpio_config = {
		.mode = GPIO_MODE_OUTPUT,
		.pin_bit_mask = 1ULL << VW_LCD_GPIO_BL };
	ESP_ERROR_CHECK( gpio_config( &bk_gpio_config ) );

	/* LCD initialization */
	ESP_LOGI( TAG, "Initialize SPI bus" );
	const spi_bus_config_t buscfg = {
		.sclk_io_num = VW_LCD_GPIO_SCLK,
		.mosi_io_num = VW_LCD_GPIO_MOSI,
		.miso_io_num = GPIO_NUM_NC,
		.quadwp_io_num = GPIO_NUM_NC,
		.quadhd_io_num = GPIO_NUM_NC,
		.max_transfer_sz = VW_LCD_H_RES * VW_LCD_DRAW_BUFF_HEIGHT * sizeof( uint16_t ),
	};
	ESP_RETURN_ON_ERROR( spi_bus_initialize( VW_LCD_SPI_NUM, &buscfg, SPI_DMA_CH_AUTO ), TAG, "SPI init failed" );

	ESP_LOGI( TAG, "Install panel IO" );
	const esp_lcd_panel_io_spi_config_t io_config = {
		.dc_gpio_num = VW_LCD_GPIO_DC,
		.cs_gpio_num = VW_LCD_GPIO_CS,
		.pclk_hz = VW_LCD_PIXEL_CLK_HZ,
		.lcd_cmd_bits = VW_LCD_CMD_BITS,
		.lcd_param_bits = VW_LCD_PARAM_BITS,
		.spi_mode = 0,
		.trans_queue_depth = 10,
	};
	ESP_GOTO_ON_ERROR( esp_lcd_new_panel_io_spi( ( esp_lcd_spi_bus_handle_t )VW_LCD_SPI_NUM, &io_config, &lcd_io ), err, TAG, "New panel IO failed" );

	ESP_LOGI( TAG, "Install LCD driver (ST7789)" );
	const esp_lcd_panel_dev_config_t panel_config = {
		.reset_gpio_num = VW_LCD_GPIO_RST,
		.rgb_endian = LCD_RGB_ENDIAN_RGB,
		.bits_per_pixel = VW_LCD_BITS_PER_PIXEL,
	};
	ESP_GOTO_ON_ERROR( esp_lcd_new_panel_st7789( lcd_io, &panel_config, &lcd_panel ), err, TAG, "New panel failed" );

	esp_lcd_panel_reset( lcd_panel );
	esp_lcd_panel_init( lcd_panel );
	esp_lcd_panel_set_gap( lcd_panel, 0, 20 );
	esp_lcd_panel_invert_color( lcd_panel, true );
	esp_lcd_panel_mirror( lcd_panel, true, true );
	esp_lcd_panel_disp_on_off( lcd_panel, true );

	/* LCD backlight on */
	ESP_ERROR_CHECK( gpio_set_level( VW_LCD_GPIO_BL, VW_LCD_BL_ON_LEVEL ) );

	// Возвращаем хэндлы через указатели
	*ret_io = lcd_io;
	*ret_panel = lcd_panel;
	return ret;

err:
	if ( lcd_panel )
		esp_lcd_panel_del( lcd_panel );
	if ( lcd_io )
		esp_lcd_panel_io_del( lcd_io );
	spi_bus_free( VW_LCD_SPI_NUM );
	return ret;
}

//-----------------------------------------------------------------------------------------
void lcd_set_power_state( bool on )
{
	if ( on )
	{
		// Включаем матрицу и подсветку
		esp_lcd_panel_disp_on_off( lcd_panel, true );
		gpio_set_level( VW_LCD_GPIO_BL, VW_LCD_BL_ON_LEVEL );
	}
	else
	{
		// Выключаем подсветку и переводим матрицу в сон
		gpio_set_level( VW_LCD_GPIO_BL, !VW_LCD_BL_ON_LEVEL );
		esp_lcd_panel_disp_on_off( lcd_panel, false );
	}
}

//-----------------------------------------------------------------------------------------
