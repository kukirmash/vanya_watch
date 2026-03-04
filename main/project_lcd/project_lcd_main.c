#include "project_lcd_main.h"

#if MOD_LVGL_LCD

LV_IMAGE_DECLARE(frame_0000);
LV_IMAGE_DECLARE(frame_0001);
LV_IMAGE_DECLARE(frame_0002);
LV_IMAGE_DECLARE(frame_0003);
LV_IMAGE_DECLARE(frame_0004);
LV_IMAGE_DECLARE(frame_0005);
LV_IMAGE_DECLARE(frame_0006);
LV_IMAGE_DECLARE(frame_0007);
LV_IMAGE_DECLARE(frame_0008);
LV_IMAGE_DECLARE(frame_0009);
LV_IMAGE_DECLARE(frame_0010);
LV_IMAGE_DECLARE(frame_0011);
LV_IMAGE_DECLARE(frame_0012);
LV_IMAGE_DECLARE(frame_0013);
LV_IMAGE_DECLARE(frame_0014);
LV_IMAGE_DECLARE(frame_0015);
LV_IMAGE_DECLARE(frame_0016);
LV_IMAGE_DECLARE(frame_0017);
LV_IMAGE_DECLARE(frame_0018);
LV_IMAGE_DECLARE(frame_0019);
LV_IMAGE_DECLARE(frame_0020);
LV_IMAGE_DECLARE(frame_0021);
LV_IMAGE_DECLARE(frame_0022);
LV_IMAGE_DECLARE(frame_0023);
LV_IMAGE_DECLARE(frame_0024);
LV_IMAGE_DECLARE(frame_0025);
LV_IMAGE_DECLARE(frame_0026);
LV_IMAGE_DECLARE(frame_0027);
LV_IMAGE_DECLARE(frame_0028);
LV_IMAGE_DECLARE(frame_0029);
LV_IMAGE_DECLARE(frame_0030);
LV_IMAGE_DECLARE(frame_0031);
LV_IMAGE_DECLARE(frame_0032);
LV_IMAGE_DECLARE(frame_0033);
LV_IMAGE_DECLARE(frame_0034);
LV_IMAGE_DECLARE(frame_0035);
LV_IMAGE_DECLARE(frame_0036);
LV_IMAGE_DECLARE(frame_0037);
LV_IMAGE_DECLARE(frame_0038);
LV_IMAGE_DECLARE(frame_0039);
LV_IMAGE_DECLARE(frame_0040);
LV_IMAGE_DECLARE(frame_0041);
LV_IMAGE_DECLARE(frame_0042);
LV_IMAGE_DECLARE(frame_0043);
LV_IMAGE_DECLARE(frame_0044);

//-----------------------------------------------------------------------------------------
static const lv_image_dsc_t * anim_imgs[45] = {
    &frame_0000, &frame_0001, &frame_0002, &frame_0003, &frame_0004,
    &frame_0005, &frame_0006, &frame_0007, &frame_0008, &frame_0009,
    &frame_0010, &frame_0011, &frame_0012, &frame_0013, &frame_0014,
    &frame_0015, &frame_0016, &frame_0017, &frame_0018, &frame_0019,
    &frame_0020, &frame_0021, &frame_0022, &frame_0023, &frame_0024,
    &frame_0025, &frame_0026, &frame_0027, &frame_0028, &frame_0029,
    &frame_0030, &frame_0031, &frame_0032, &frame_0033, &frame_0034,
    &frame_0035, &frame_0036, &frame_0037, &frame_0038, &frame_0039,
    &frame_0040, &frame_0041, &frame_0042, &frame_0043, &frame_0044
};

//-----------------------------------------------------------------------------------------
void lvgl_lcd()
{
    lv_obj_t * animimg0 = lv_animimg_create(lv_screen_active());
    lv_obj_center(animimg0);
    lv_animimg_set_src(animimg0, (const void **) anim_imgs, 45);
    lv_animimg_set_duration(animimg0, 1500);
    lv_animimg_set_repeat_count(animimg0, LV_ANIM_REPEAT_INFINITE);
    lv_animimg_start(animimg0);
}

//-----------------------------------------------------------------------------------------

#endif // MOD_LVGL_LCD