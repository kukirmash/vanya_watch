#include "power.h"

#include "driver/gpio.h"

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_log.h"
#include "esp_sleep.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "board/lcd.h"

static const char *TAG = "POWER";

static adc_oneshot_unit_handle_t adc1_handle;
static adc_cali_handle_t adc1_cali_handle = NULL;
static bool do_calibration = false;

// Сохраняем последний процент, чтобы он резко не подскакивал при зарядке
static uint8_t last_known_percent = 0;
static bool has_initial_reading = false;

// Глобальная переменная состояния устройства (для обработки сна)
bool is_screen_on = true;

//-----------------------------------------------------------------------------------------
static void power_task(void *pvParameter)
{
    gpio_config_t btn_cfg =
        {
            .pin_bit_mask = (1ULL << VW_GPIO_BUTTON),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_ENABLE};
    gpio_config(&btn_cfg);

    vTaskDelay(pdMS_TO_TICKS(1000)); // Защита при старте

    bool btn_prev = true; // 1 = кнопка НЕ нажата (из-за pull-up)

    while (1)
    {
        // 1. Проверка на севшую батарею: выключаем полностью, если <= 0%
        if (has_initial_reading && last_known_percent == 0 && !power_get_battery_is_charging())
        {
            ESP_LOGW(TAG, "Battery 0%. Forced hardware shutdown!");
            power_shutdown();
        }

        // 2. Обработка одиночного нажатия (Ловим момент ОТПУСКАНИЯ кнопки)
        bool btn_now = gpio_get_level(VW_GPIO_BUTTON);

        if (btn_now == 1 && btn_prev == 0) // Переход от 0 (нажата) к 1 (отпущена)
        {
            if (is_screen_on)
            {
                ESP_LOGI(TAG, "Screen OFF");
                lcd_set_power_state(false);
                is_screen_on = false;
            }
            else
            {
                ESP_LOGI(TAG, "Screen ON");
                lcd_set_power_state(true);
                is_screen_on = true;
            }
        }

        btn_prev = btn_now;
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

//-----------------------------------------------------------------------------------------
esp_err_t power_init(void)
{
    // 1. Перехват питания (сразу!)
    gpio_config_t hold_cfg =
        {
            .pin_bit_mask = (1ULL << VW_GPIO_POWER_HOLD),
            .mode = GPIO_MODE_OUTPUT};
    gpio_config(&hold_cfg);
    gpio_set_level(VW_GPIO_POWER_HOLD, 1);

    // 2. Инициализация пина зарядки
    gpio_config_t chg_cfg =
        {
            .pin_bit_mask = (1ULL << VW_GPIO_CHG_STAT),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_ENABLE, // Если всегда false, возможно нужен GPIO_PULLDOWN_ENABLE
            .pull_down_en = GPIO_PULLDOWN_DISABLE};
    gpio_config(&chg_cfg);

    // 3. Инициализация АЦП
    adc_oneshot_unit_init_cfg_t init_config1 = {.unit_id = ADC_UNIT_1};
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    adc_oneshot_chan_cfg_t config =
        {
            .bitwidth = ADC_BITWIDTH_DEFAULT,
            .atten = ADC_ATTEN_DB_11,
        };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, VW_GPIO_BAT_ADC, &config));

    // Калибровка
    adc_cali_curve_fitting_config_t cali_config =
        {
            .unit_id = ADC_UNIT_1,
            .atten = ADC_ATTEN_DB_11,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
    if (adc_cali_create_scheme_curve_fitting(&cali_config, &adc1_cali_handle) == ESP_OK)
        do_calibration = true;

    // Первичное чтение для инициализации
    power_get_battery_percent();

    // 4. Запуск задачи мониторинга кнопки
    xTaskCreate(power_task, "power_task", 2048, NULL, 10, NULL);

    return ESP_OK;
}

//-----------------------------------------------------------------------------------------
uint8_t power_get_battery_percent(void)
{
    int raw_sum = 0;
    int samples = 10;

    // Oversampling (Многократная выборка для устранения скачков)
    for (int i = 0; i < samples; i++)
    {
        int val = 0;
        adc_oneshot_read(adc1_handle, VW_GPIO_BAT_ADC, &val);
        raw_sum += val;
        vTaskDelay(pdMS_TO_TICKS(5)); // Пауза 2 мс между замерами
    }

    int adc_raw = raw_sum / samples; // Среднее значение
    int voltage_mv = 0;

    // Переводим в миллиВольты
    if (do_calibration)
        adc_cali_raw_to_voltage(adc1_cali_handle, adc_raw, &voltage_mv);
    else
        voltage_mv = (adc_raw * 3100) / 4095;

    int battery_mv = voltage_mv * 3; // Делитель напряжения 1/3

    // Если заряжается, не вычисляем заряд, а возвращаем старое значение
    // (Потому что напряжение на клеммах завышено зарядником)
    if (has_initial_reading && power_get_battery_is_charging())
        return last_known_percent;

    const int curve_points = 8;
    const int voltage_curve[] = {4140, 3950, 3880, 3810, 3710, 3560, 3400, 3200};
    const int percent_curve[] = {100, 90, 80, 70, 50, 30, 10, 0};

    uint8_t calculated_percent = 0;

    if (battery_mv >= voltage_curve[0])
        calculated_percent = 100;
    else if (battery_mv <= voltage_curve[curve_points - 1])
        calculated_percent = 0;
    else
    {
        // Линейная интерполяция
        for (int i = 0; i < curve_points - 1; i++)
        {
            if (battery_mv <= voltage_curve[i] && battery_mv > voltage_curve[i + 1])
            {
                int v_range = voltage_curve[i] - voltage_curve[i + 1];
                int p_range = percent_curve[i] - percent_curve[i + 1];
                int v_offset = battery_mv - voltage_curve[i + 1];

                calculated_percent = percent_curve[i + 1] + (v_offset * p_range) / v_range;
                break;
            }
        }
    }

    last_known_percent = calculated_percent;
    has_initial_reading = true;

    return calculated_percent;
}

//-----------------------------------------------------------------------------------------
bool power_get_battery_is_charging(void)
{
    return gpio_get_level(VW_GPIO_CHG_STAT) == 0;
}

//-----------------------------------------------------------------------------------------
void power_shutdown(void)
{
    gpio_set_level(VW_GPIO_POWER_HOLD, 0); // Обесточивает плату аппаратно
}

//-----------------------------------------------------------------------------------------
// Получить уровень заряда в мили Вольтах mV
uint32_t power_get_battery_mvolt(void)
{
    int raw_sum = 0;
    int samples = 10;

    // Oversampling (Многократная выборка для устранения скачков)
    for (int i = 0; i < samples; i++)
    {
        int val = 0;
        adc_oneshot_read(adc1_handle, VW_GPIO_BAT_ADC, &val);
        raw_sum += val;
        vTaskDelay(pdMS_TO_TICKS(2)); // Пауза 2 мс между замерами
    }

    int adc_raw = raw_sum / samples; // Среднее значение
    int voltage_mv = 0;

    // Переводим в миллиВольты
    if (do_calibration)
        adc_cali_raw_to_voltage(adc1_cali_handle, adc_raw, &voltage_mv);
    else
        voltage_mv = (adc_raw * 3100) / 4095;

    int battery_mv = voltage_mv * 3; // Делитель напряжения 1/3

    return battery_mv;
}

//-----------------------------------------------------------------------------------------
