#include "app_ui/windows/watchface.h"

#include "app_ui/animations/watchface_anims/watchface_anim_1.h"
#include "helpers/time/time_helper.h"
#include "board/time.h"

static const char *TAG = "WATCHFACE";

//-----------------------------------------------------------------------------------------
static lv_obj_t *anim_h_tens;  // Десятки часов
static lv_obj_t *anim_h_units; // Единицы часов
static lv_obj_t *anim_m_tens;  // Десятки минут
static lv_obj_t *anim_m_units; // Единицы минут

static lv_obj_t *date_label;	// Дата
static lv_obj_t *battery_label; // Заряд батареи
static lv_obj_t *wifi_label;	// Иконка Wi-Fi

static uint8_t curr_h_tens = 255;
static uint8_t curr_h_units = 255;
static uint8_t curr_m_tens = 255;
static uint8_t curr_m_units = 255;
static uint8_t curr_day = 0;

static int last_min = -1;

static const char *month_names[] = {
	"January", "February", "March", "April", "May", "June",
	"July", "August", "September", "October", "November", "December"};

//-----------------------------------------------------------------------------------------
// Возвращает нужный массив кадров анимации в зависимости от начальной и конечной цифры
static const lv_image_dsc_t **get_anim_array(uint8_t from_digit, uint8_t to_digit)
{
	if (from_digit == 0 && to_digit == 1)
		return (const lv_image_dsc_t **)anim_01_imgs;
	if (from_digit == 1 && to_digit == 2)
		return (const lv_image_dsc_t **)anim_12_imgs;
	if (from_digit == 2 && to_digit == 3)
		return (const lv_image_dsc_t **)anim_23_imgs;
	if (from_digit == 3 && to_digit == 4)
		return (const lv_image_dsc_t **)anim_34_imgs;
	if (from_digit == 4 && to_digit == 5)
		return (const lv_image_dsc_t **)anim_45_imgs;
	if (from_digit == 5 && to_digit == 6)
		return (const lv_image_dsc_t **)anim_56_imgs;
	if (from_digit == 6 && to_digit == 7)
		return (const lv_image_dsc_t **)anim_67_imgs;
	if (from_digit == 7 && to_digit == 8)
		return (const lv_image_dsc_t **)anim_78_imgs;
	if (from_digit == 8 && to_digit == 9)
		return (const lv_image_dsc_t **)anim_89_imgs;
	if (from_digit == 9 && to_digit == 0)
		return (const lv_image_dsc_t **)anim_90_imgs;

	if (from_digit == 2 && to_digit == 0)
		return (const lv_image_dsc_t **)anim_20_imgs;
	if (from_digit == 3 && to_digit == 0)
		return (const lv_image_dsc_t **)anim_30_imgs;
	if (from_digit == 5 && to_digit == 0)
		return (const lv_image_dsc_t **)anim_50_imgs;

	return (const lv_image_dsc_t **)anim_01_imgs;
}

//-----------------------------------------------------------------------------------------
static void play_transition(lv_obj_t *anim_obj, uint8_t from_digit, uint8_t to_digit)
{
	const lv_image_dsc_t **animation = get_anim_array(from_digit, to_digit);
	lv_animimg_set_src(anim_obj, (const void **)animation, 46);
	lv_animimg_start(anim_obj);
}

//-----------------------------------------------------------------------------------------
// Обновляет значение цифры на экране с запуском анимации или статической установкой
static void update_digit(lv_obj_t *anim_obj, uint8_t *curr_val, uint8_t new_val, uint8_t prev_val)
{
	// Прерывание, если значение не изменилось
	if (*curr_val == new_val)
		return;

	// Установка последнего кадра анимации перехода в качестве начального статического состояния
	if (*curr_val == 255)
		play_transition(anim_obj, prev_val, new_val);
	else // Запуск полноценной анимации перехода
		play_transition(anim_obj, *curr_val, new_val);

	*curr_val = new_val;
}

//-----------------------------------------------------------------------------------------
// Обрабатывает событие изменения времени и обновляет элементы интерфейса
static void watchface_time_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
	// Получаем timestamp из Subject
	time_t current_time = (time_t)lv_subject_get_int(subject);
	if (current_time == 0)
		return;

	// Безопасно распаковываем время в структуру прямо внутри потока отрисовки
	struct tm time_info;
	localtime_r(&current_time, &time_info);

	// Ограничение частоты обновления интерфейса до одного раза в минуту (после первичной инициализации)
	if (last_min == time_info.tm_min && curr_h_tens != 255)
		return;

	last_min = time_info.tm_min;

	// Расчет текущих значений для каждой позиции
	uint8_t new_h_tens = time_info.tm_hour / 10;
	uint8_t new_h_units = time_info.tm_hour % 10;
	uint8_t new_m_tens = time_info.tm_min / 10;
	uint8_t new_m_units = time_info.tm_min % 10;

	// Вычисление предыдущих значений для корректного выбора начального кадра при запуске
	uint8_t prev_h_tens = get_prev_h_tens(new_h_tens);
	uint8_t prev_h_units = get_prev_h_units(new_h_units, new_h_tens);
	uint8_t prev_m_tens = get_prev_m_tens(new_m_tens);
	uint8_t prev_m_units = get_prev_m_units(new_m_units);

	// Применение изменений к инерфейсу и к переменным
	update_digit(anim_h_tens, &curr_h_tens, new_h_tens, prev_h_tens);
	update_digit(anim_h_units, &curr_h_units, new_h_units, prev_h_units);
	update_digit(anim_m_tens, &curr_m_tens, new_m_tens, prev_m_tens);
	update_digit(anim_m_units, &curr_m_units, new_m_units, prev_m_units);

	// Обновление текстового поля с датой только при смене дня в году
	if (curr_day != time_info.tm_yday)
	{
		lv_label_set_text_fmt(date_label, "%02d %s", time_info.tm_mday, month_names[time_info.tm_mon]);
		curr_day = time_info.tm_yday;
	}
}

//-----------------------------------------------------------------------------------------
void watchface_init(lv_obj_t *parent)
{
	// Дата
	date_label = lv_label_create(parent);
	lv_label_set_text(date_label, "");
	lv_obj_align(date_label, LV_ALIGN_TOP_MID, 0, 10);
	lv_obj_set_style_text_font(date_label, VW_FONT_18, LV_PART_MAIN);
	lv_obj_set_style_text_color(date_label, lv_color_hex(VW_PRIMARY_COLOR_HEX), LV_PART_MAIN);

	// Часы - десятки
	anim_h_tens = lv_animimg_create(parent);
	lv_obj_align(anim_h_tens, LV_ALIGN_TOP_LEFT, 7, 27);
	lv_animimg_set_duration(anim_h_tens, 1500);
	lv_animimg_set_repeat_count(anim_h_tens, 1);
	lv_animimg_set_src(anim_h_tens, (const void **)anim_01_imgs, 46);
	lv_obj_set_style_image_recolor(anim_h_tens, lv_color_hex(VW_PRIMARY_COLOR_HEX), LV_PART_MAIN);
	lv_obj_set_style_image_recolor_opa(anim_h_tens, 255, LV_PART_MAIN);

	// Часы - единицы
	anim_h_units = lv_animimg_create(parent);
	lv_obj_align(anim_h_units, LV_ALIGN_TOP_RIGHT, -7, 27);
	lv_animimg_set_duration(anim_h_units, 1500);
	lv_animimg_set_repeat_count(anim_h_units, 1);
	lv_animimg_set_src(anim_h_units, (const void **)anim_01_imgs, 46);
	lv_obj_set_style_image_recolor(anim_h_units, lv_color_hex(VW_PRIMARY_COLOR_HEX), LV_PART_MAIN);
	lv_obj_set_style_image_recolor_opa(anim_h_units, 255, LV_PART_MAIN);

	// Минуты - десятки
	anim_m_tens = lv_animimg_create(parent);
	lv_obj_align(anim_m_tens, LV_ALIGN_BOTTOM_LEFT, 7, -27);
	lv_animimg_set_duration(anim_m_tens, 1500);
	lv_animimg_set_repeat_count(anim_m_tens, 1);
	lv_animimg_set_src(anim_m_tens, (const void **)anim_01_imgs, 46);
	lv_obj_set_style_image_recolor(anim_m_tens, lv_color_hex(VW_PRIMARY_COLOR_HEX), LV_PART_MAIN);
	lv_obj_set_style_image_recolor_opa(anim_m_tens, 255, LV_PART_MAIN);

	// Минуты - единицы
	anim_m_units = lv_animimg_create(parent);
	lv_obj_align(anim_m_units, LV_ALIGN_BOTTOM_RIGHT, -7, -27);
	lv_animimg_set_duration(anim_m_units, 1500);
	lv_animimg_set_repeat_count(anim_m_units, 1);
	lv_animimg_set_src(anim_m_units, (const void **)anim_01_imgs, 46);
	lv_obj_set_style_image_recolor(anim_m_units, lv_color_hex(VW_PRIMARY_COLOR_HEX), LV_PART_MAIN);
	lv_obj_set_style_image_recolor_opa(anim_m_units, 255, LV_PART_MAIN);

	lv_obj_t *bottom_cont = lv_obj_create(parent);
	lv_obj_set_size(bottom_cont, lv_pct(100), 35);
	lv_obj_align(bottom_cont, LV_ALIGN_BOTTOM_MID, 0, 0);
	lv_obj_set_layout(bottom_cont, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(bottom_cont, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(bottom_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_style_bg_opa(bottom_cont, 0, LV_PART_MAIN);
	lv_obj_set_style_border_width(bottom_cont, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_all(bottom_cont, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_gap(bottom_cont, 10, LV_PART_MAIN);

	// Wi-Fi
	wifi_label = lv_label_create(bottom_cont);
	lv_label_set_text(wifi_label, "");
	lv_obj_set_style_text_font(wifi_label, VW_FONT_18, LV_PART_MAIN);
	lv_obj_set_style_text_color(wifi_label, lv_color_hex(VW_PRIMARY_COLOR_HEX), LV_PART_MAIN);

	// Заряд
	battery_label = lv_label_create(bottom_cont);
	lv_label_set_text(battery_label, "");
	lv_obj_set_style_text_font(battery_label, VW_FONT_18, LV_PART_MAIN);
	lv_obj_set_style_text_color(battery_label, lv_color_hex(VW_PRIMARY_COLOR_HEX), LV_PART_MAIN);

	// Подписка на обновление времени и даты
	lv_subject_add_observer_obj(&subject_time, watchface_time_observer_cb, parent, NULL);

	// lv_label_bind_text(date_label, &subject_date_str, NULL);
	// lv_label_bind_text(battery_label, &subject_power_str, NULL);
	// lv_subject_add_observer_obj(&subject_time, watchface_time_observer_cb, parent, NULL);
	// lv_subject_add_observer_obj(&subject_wifi, watchface_wifi_observer_cb, wifi_label, NULL); // TODO: bind_text на икноку
}

//-----------------------------------------------------------------------------------------