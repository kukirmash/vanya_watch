#include "power.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "POWER";

static adc_oneshot_unit_handle_t adc1_handle;
static adc_cali_handle_t adc1_cali_handle = NULL;
static bool do_calibration = false;

//-----------------------------------------------------------------------------------------
// Внутренняя задача для отслеживания долгого нажатия кнопки выключения
static void button_monitor_task(void *pvParameter)
{
    gpio_config_t btn_cfg = {
        .pin_bit_mask = (1ULL << VW_GPIO_BUTTON),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE};
    gpio_config(&btn_cfg);

    vTaskDelay(pdMS_TO_TICKS(2000)); // Защита при старте

    int hold_time = 0;
    while (1)
    {
        if (gpio_get_level(VW_GPIO_BUTTON) == 0)
        {
            hold_time += 100;
            if (hold_time >= 1500)
            {
                ESP_LOGI(TAG, "Shutting down...");
                power_shutdown();
            }
        }
        else
        {
            hold_time = 0;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

//-----------------------------------------------------------------------------------------
esp_err_t power_init(void)
{
    // 1. Перехват питания (сразу!)
    gpio_config_t hold_cfg = {
        .pin_bit_mask = (1ULL << VW_GPIO_POWER_HOLD),
        .mode = GPIO_MODE_OUTPUT};
    gpio_config(&hold_cfg);
    gpio_set_level(VW_GPIO_POWER_HOLD, 1);

    // 2. Инициализация пина зарядки (GPIO 2)
    gpio_config_t chg_cfg = {
        .pin_bit_mask = (1ULL << VW_GPIO_CHG_STAT),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE};
    gpio_config(&chg_cfg);

    // 3. Инициализация АЦП для батареи
    adc_oneshot_unit_init_cfg_t init_config1 = {.unit_id = ADC_UNIT_1};
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, VW_GPIO_BAT_ADC, &config));

    // Калибровка
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_11,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    if (adc_cali_create_scheme_curve_fitting(&cali_config, &adc1_cali_handle) == ESP_OK)
    {
        do_calibration = true;
    }

    // 4. Запуск задачи мониторинга кнопки
    xTaskCreate(button_monitor_task, "btn_task", 2048, NULL, 10, NULL);

    return ESP_OK;
}

//-----------------------------------------------------------------------------------------
uint8_t power_get_battery_percent(void)
{
    int adc_raw=0;
    int voltage_mv=0;
    
    // Получаем значение с АЦП
    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, VW_GPIO_BAT_ADC, &adc_raw)); 

    // Переводим в миллиВольты
    if (do_calibration)
        adc_cali_raw_to_voltage(adc1_cali_handle, adc_raw, &voltage_mv);
    else
        voltage_mv = (adc_raw * 3100) / 4095;

    int battery_mv = voltage_mv * 3; // Делитель напряжения 1/3

    //ESP_LOGI(TAG, "adc_raw: %d", adc_raw);
    printf(">>> ADC RAW: %d | VOLTAGE: %d mV | BATTERY: %d mV <<<\n", adc_raw, voltage_mv, battery_mv); 

    if (battery_mv >= 4200)
        return 100;
    if (battery_mv <= 3300)
        return 0;
    return (battery_mv - 3300) / 9;
}

//-----------------------------------------------------------------------------------------
bool battery_is_charging(void)
{
    // Инвертированная логика: 0 - заряжается 
    return gpio_get_level(VW_GPIO_CHG_STAT) == 0;
}

//-----------------------------------------------------------------------------------------
void power_shutdown(void)
{
    gpio_set_level(VW_GPIO_POWER_HOLD, 0);
}

//-----------------------------------------------------------------------------------------
