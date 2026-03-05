#include "project_lcd_main.h"

#if MOD_LVGL_LCD

LV_IMAGE_DECLARE(img0001);
LV_IMAGE_DECLARE(img0002);
LV_IMAGE_DECLARE(img0003);
LV_IMAGE_DECLARE(img0004);
LV_IMAGE_DECLARE(img0005);
LV_IMAGE_DECLARE(img0006);
LV_IMAGE_DECLARE(img0007);
LV_IMAGE_DECLARE(img0008);
LV_IMAGE_DECLARE(img0009);
LV_IMAGE_DECLARE(img0010);
LV_IMAGE_DECLARE(img0011);
LV_IMAGE_DECLARE(img0012);
LV_IMAGE_DECLARE(img0013);
LV_IMAGE_DECLARE(img0014);
LV_IMAGE_DECLARE(img0015);
LV_IMAGE_DECLARE(img0016);
LV_IMAGE_DECLARE(img0017);
LV_IMAGE_DECLARE(img0018);
LV_IMAGE_DECLARE(img0019);
LV_IMAGE_DECLARE(img0020);
LV_IMAGE_DECLARE(img0021);
LV_IMAGE_DECLARE(img0022);
LV_IMAGE_DECLARE(img0023);
LV_IMAGE_DECLARE(img0024);
LV_IMAGE_DECLARE(img0025);
LV_IMAGE_DECLARE(img0026);
LV_IMAGE_DECLARE(img0027);
LV_IMAGE_DECLARE(img0028);
LV_IMAGE_DECLARE(img0029);
LV_IMAGE_DECLARE(img0030);
LV_IMAGE_DECLARE(img0031);
LV_IMAGE_DECLARE(img0032);
LV_IMAGE_DECLARE(img0033);
LV_IMAGE_DECLARE(img0034);
LV_IMAGE_DECLARE(img0035);
LV_IMAGE_DECLARE(img0036);
LV_IMAGE_DECLARE(img0037);
LV_IMAGE_DECLARE(img0038);
LV_IMAGE_DECLARE(img0039);
LV_IMAGE_DECLARE(img0040);
LV_IMAGE_DECLARE(img0041);
LV_IMAGE_DECLARE(img0042);
LV_IMAGE_DECLARE(img0043);
LV_IMAGE_DECLARE(img0044);
LV_IMAGE_DECLARE(img0045);
LV_IMAGE_DECLARE(img0046);

//-----------------------------------------------------------------------------------------
static const lv_image_dsc_t * anim_imgs[46] = {
    &img0001, &img0002, &img0003, &img0004,
    &img0005, &img0006, &img0007, &img0008, &img0009,
    &img0010, &img0011, &img0012, &img0013, &img0014,
    &img0015, &img0016, &img0017, &img0018, &img0019,
    &img0020, &img0021, &img0022, &img0023, &img0024,
    &img0025, &img0026, &img0027, &img0028, &img0029,
    &img0030, &img0031, &img0032, &img0033, &img0034,
    &img0035, &img0036, &img0037, &img0038, &img0039,
    &img0040, &img0041, &img0042, &img0043, &img0044,
    &img0045, &img0046
};

//-----------------------------------------------------------------------------------------
void lvgl_lcd()
{
    lv_obj_t * animimg0 = lv_animimg_create(lv_screen_active());
    lv_obj_align(animimg0, LV_ALIGN_CENTER, -50, -50);
    lv_animimg_set_src(animimg0, (const void **) anim_imgs, 46);
    lv_animimg_set_duration(animimg0, 1500);
    lv_animimg_set_repeat_count(animimg0, LV_ANIM_REPEAT_INFINITE);
    lv_animimg_start(animimg0);

    lv_obj_t * animimg1 = lv_animimg_create(lv_screen_active());
    lv_obj_align(animimg1, LV_ALIGN_CENTER, 50, -50);
    lv_animimg_set_src(animimg1, (const void **) anim_imgs, 46);
    lv_animimg_set_duration(animimg1, 1500);
    lv_animimg_set_repeat_count(animimg1, LV_ANIM_REPEAT_INFINITE);
    lv_animimg_start(animimg1);
    
    lv_obj_t * animimg2 = lv_animimg_create(lv_screen_active());
    lv_obj_align(animimg2, LV_ALIGN_CENTER, -50, 50);
    lv_animimg_set_src(animimg2, (const void **) anim_imgs, 46);
    lv_animimg_set_duration(animimg2, 1500);
    lv_animimg_set_repeat_count(animimg2, LV_ANIM_REPEAT_INFINITE);
    lv_animimg_start(animimg2);

    lv_obj_t * animimg3 = lv_animimg_create(lv_screen_active());
    lv_obj_align(animimg3, LV_ALIGN_CENTER, 50, 50);
    lv_animimg_set_src(animimg3, (const void **) anim_imgs, 46);
    lv_animimg_set_duration(animimg3, 1500);
    lv_animimg_set_repeat_count(animimg3, LV_ANIM_REPEAT_INFINITE);
    lv_animimg_start(animimg3);
}

//-----------------------------------------------------------------------------------------

#endif // MOD_LVGL_LCD