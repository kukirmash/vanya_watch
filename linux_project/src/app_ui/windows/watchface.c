#include "app_ui/windows/watchface.h"

#include "app_ui/animations/watchface_anims/watchface_anim_1.h"

static const char *TAG = "WATCHFACE";

#include "helpers/time/time_helper.h"

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

static const char *month_names[] = {
	"January", "February", "March", "April", "May", "June",
	"July", "August", "September", "October", "November", "December"};

//-----------------------------------------------------------------------------------------
// Нужная анимация в зависимости от перехода цифры
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
	const lv_image_dsc_t** animation = get_anim_array(from_digit, to_digit);
	lv_animimg_set_src(anim_obj, (const void **)animation, 46);
	lv_animimg_start(anim_obj);
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
	lv_obj_set_style_text_font(battery_label, VW_FONT_18, LV_PART_MAIN);
	lv_obj_set_style_text_color(battery_label, lv_color_hex(VW_PRIMARY_COLOR_HEX), LV_PART_MAIN);

	// lv_label_bind_text(date_label, &subject_date_str, NULL);
	// lv_label_bind_text(battery_label, &subject_power_str, NULL);
	// lv_subject_add_observer_obj(&subject_time, watchface_time_observer_cb, parent, NULL);
	// lv_subject_add_observer_obj(&subject_wifi, watchface_wifi_observer_cb, wifi_label, NULL); // TODO: bind_text на икноку
}

//-----------------------------------------------------------------------------------------
