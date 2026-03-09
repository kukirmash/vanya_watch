#include "project_lcd_main.h"

#if MOD_LVGL_LCD

//-----------------------------------------------------------------------------------------
// 0-1
LV_IMAGE_DECLARE(img_0_1_01);LV_IMAGE_DECLARE(img_0_1_02);LV_IMAGE_DECLARE(img_0_1_03);LV_IMAGE_DECLARE(img_0_1_04);
LV_IMAGE_DECLARE(img_0_1_05);LV_IMAGE_DECLARE(img_0_1_06);LV_IMAGE_DECLARE(img_0_1_07);LV_IMAGE_DECLARE(img_0_1_08);
LV_IMAGE_DECLARE(img_0_1_09);LV_IMAGE_DECLARE(img_0_1_10);LV_IMAGE_DECLARE(img_0_1_11);LV_IMAGE_DECLARE(img_0_1_12);
LV_IMAGE_DECLARE(img_0_1_13);LV_IMAGE_DECLARE(img_0_1_14);LV_IMAGE_DECLARE(img_0_1_15);LV_IMAGE_DECLARE(img_0_1_16);
LV_IMAGE_DECLARE(img_0_1_17);LV_IMAGE_DECLARE(img_0_1_18);LV_IMAGE_DECLARE(img_0_1_19);LV_IMAGE_DECLARE(img_0_1_20);
LV_IMAGE_DECLARE(img_0_1_21);LV_IMAGE_DECLARE(img_0_1_22);LV_IMAGE_DECLARE(img_0_1_23);LV_IMAGE_DECLARE(img_0_1_24);
LV_IMAGE_DECLARE(img_0_1_25);LV_IMAGE_DECLARE(img_0_1_26);LV_IMAGE_DECLARE(img_0_1_27);LV_IMAGE_DECLARE(img_0_1_28);
LV_IMAGE_DECLARE(img_0_1_29);LV_IMAGE_DECLARE(img_0_1_30);LV_IMAGE_DECLARE(img_0_1_31);LV_IMAGE_DECLARE(img_0_1_32);
LV_IMAGE_DECLARE(img_0_1_33);LV_IMAGE_DECLARE(img_0_1_34);LV_IMAGE_DECLARE(img_0_1_35);LV_IMAGE_DECLARE(img_0_1_36);
LV_IMAGE_DECLARE(img_0_1_37);LV_IMAGE_DECLARE(img_0_1_38);LV_IMAGE_DECLARE(img_0_1_39);LV_IMAGE_DECLARE(img_0_1_40);
LV_IMAGE_DECLARE(img_0_1_41);LV_IMAGE_DECLARE(img_0_1_42);LV_IMAGE_DECLARE(img_0_1_43);LV_IMAGE_DECLARE(img_0_1_44);
LV_IMAGE_DECLARE(img_0_1_45);LV_IMAGE_DECLARE(img_0_1_46);

//-----------------------------------------------------------------------------------------
// 1-2
LV_IMAGE_DECLARE(img_1_2_01);LV_IMAGE_DECLARE(img_1_2_02);LV_IMAGE_DECLARE(img_1_2_03);LV_IMAGE_DECLARE(img_1_2_04);
LV_IMAGE_DECLARE(img_1_2_05);LV_IMAGE_DECLARE(img_1_2_06);LV_IMAGE_DECLARE(img_1_2_07);LV_IMAGE_DECLARE(img_1_2_08);
LV_IMAGE_DECLARE(img_1_2_09);LV_IMAGE_DECLARE(img_1_2_10);LV_IMAGE_DECLARE(img_1_2_11);LV_IMAGE_DECLARE(img_1_2_12);
LV_IMAGE_DECLARE(img_1_2_13);LV_IMAGE_DECLARE(img_1_2_14);LV_IMAGE_DECLARE(img_1_2_15);LV_IMAGE_DECLARE(img_1_2_16);
LV_IMAGE_DECLARE(img_1_2_17);LV_IMAGE_DECLARE(img_1_2_18);LV_IMAGE_DECLARE(img_1_2_19);LV_IMAGE_DECLARE(img_1_2_20);
LV_IMAGE_DECLARE(img_1_2_21);LV_IMAGE_DECLARE(img_1_2_22);LV_IMAGE_DECLARE(img_1_2_23);LV_IMAGE_DECLARE(img_1_2_24);
LV_IMAGE_DECLARE(img_1_2_25);LV_IMAGE_DECLARE(img_1_2_26);LV_IMAGE_DECLARE(img_1_2_27);LV_IMAGE_DECLARE(img_1_2_28);
LV_IMAGE_DECLARE(img_1_2_29);LV_IMAGE_DECLARE(img_1_2_30);LV_IMAGE_DECLARE(img_1_2_31);LV_IMAGE_DECLARE(img_1_2_32);
LV_IMAGE_DECLARE(img_1_2_33);LV_IMAGE_DECLARE(img_1_2_34);LV_IMAGE_DECLARE(img_1_2_35);LV_IMAGE_DECLARE(img_1_2_36);
LV_IMAGE_DECLARE(img_1_2_37);LV_IMAGE_DECLARE(img_1_2_38);LV_IMAGE_DECLARE(img_1_2_39);LV_IMAGE_DECLARE(img_1_2_40);
LV_IMAGE_DECLARE(img_1_2_41);LV_IMAGE_DECLARE(img_1_2_42);LV_IMAGE_DECLARE(img_1_2_43);LV_IMAGE_DECLARE(img_1_2_44);
LV_IMAGE_DECLARE(img_1_2_45);LV_IMAGE_DECLARE(img_1_2_46);

//-----------------------------------------------------------------------------------------
// 2-3
LV_IMAGE_DECLARE(img_2_3_01);LV_IMAGE_DECLARE(img_2_3_02);LV_IMAGE_DECLARE(img_2_3_03);LV_IMAGE_DECLARE(img_2_3_04);
LV_IMAGE_DECLARE(img_2_3_05);LV_IMAGE_DECLARE(img_2_3_06);LV_IMAGE_DECLARE(img_2_3_07);LV_IMAGE_DECLARE(img_2_3_08);
LV_IMAGE_DECLARE(img_2_3_09);LV_IMAGE_DECLARE(img_2_3_10);LV_IMAGE_DECLARE(img_2_3_11);LV_IMAGE_DECLARE(img_2_3_12);
LV_IMAGE_DECLARE(img_2_3_13);LV_IMAGE_DECLARE(img_2_3_14);LV_IMAGE_DECLARE(img_2_3_15);LV_IMAGE_DECLARE(img_2_3_16);
LV_IMAGE_DECLARE(img_2_3_17);LV_IMAGE_DECLARE(img_2_3_18);LV_IMAGE_DECLARE(img_2_3_19);LV_IMAGE_DECLARE(img_2_3_20);
LV_IMAGE_DECLARE(img_2_3_21);LV_IMAGE_DECLARE(img_2_3_22);LV_IMAGE_DECLARE(img_2_3_23);LV_IMAGE_DECLARE(img_2_3_24);
LV_IMAGE_DECLARE(img_2_3_25);LV_IMAGE_DECLARE(img_2_3_26);LV_IMAGE_DECLARE(img_2_3_27);LV_IMAGE_DECLARE(img_2_3_28);
LV_IMAGE_DECLARE(img_2_3_29);LV_IMAGE_DECLARE(img_2_3_30);LV_IMAGE_DECLARE(img_2_3_31);LV_IMAGE_DECLARE(img_2_3_32);
LV_IMAGE_DECLARE(img_2_3_33);LV_IMAGE_DECLARE(img_2_3_34);LV_IMAGE_DECLARE(img_2_3_35);LV_IMAGE_DECLARE(img_2_3_36);
LV_IMAGE_DECLARE(img_2_3_37);LV_IMAGE_DECLARE(img_2_3_38);LV_IMAGE_DECLARE(img_2_3_39);LV_IMAGE_DECLARE(img_2_3_40);
LV_IMAGE_DECLARE(img_2_3_41);LV_IMAGE_DECLARE(img_2_3_42);LV_IMAGE_DECLARE(img_2_3_43);LV_IMAGE_DECLARE(img_2_3_44);
LV_IMAGE_DECLARE(img_2_3_45);LV_IMAGE_DECLARE(img_2_3_46);

//-----------------------------------------------------------------------------------------
// 3-4
LV_IMAGE_DECLARE(img_3_4_01);LV_IMAGE_DECLARE(img_3_4_02);LV_IMAGE_DECLARE(img_3_4_03);LV_IMAGE_DECLARE(img_3_4_04);
LV_IMAGE_DECLARE(img_3_4_05);LV_IMAGE_DECLARE(img_3_4_06);LV_IMAGE_DECLARE(img_3_4_07);LV_IMAGE_DECLARE(img_3_4_08);
LV_IMAGE_DECLARE(img_3_4_09);LV_IMAGE_DECLARE(img_3_4_10);LV_IMAGE_DECLARE(img_3_4_11);LV_IMAGE_DECLARE(img_3_4_12);
LV_IMAGE_DECLARE(img_3_4_13);LV_IMAGE_DECLARE(img_3_4_14);LV_IMAGE_DECLARE(img_3_4_15);LV_IMAGE_DECLARE(img_3_4_16);
LV_IMAGE_DECLARE(img_3_4_17);LV_IMAGE_DECLARE(img_3_4_18);LV_IMAGE_DECLARE(img_3_4_19);LV_IMAGE_DECLARE(img_3_4_20);
LV_IMAGE_DECLARE(img_3_4_21);LV_IMAGE_DECLARE(img_3_4_22);LV_IMAGE_DECLARE(img_3_4_23);LV_IMAGE_DECLARE(img_3_4_24);
LV_IMAGE_DECLARE(img_3_4_25);LV_IMAGE_DECLARE(img_3_4_26);LV_IMAGE_DECLARE(img_3_4_27);LV_IMAGE_DECLARE(img_3_4_28);
LV_IMAGE_DECLARE(img_3_4_29);LV_IMAGE_DECLARE(img_3_4_30);LV_IMAGE_DECLARE(img_3_4_31);LV_IMAGE_DECLARE(img_3_4_32);
LV_IMAGE_DECLARE(img_3_4_33);LV_IMAGE_DECLARE(img_3_4_34);LV_IMAGE_DECLARE(img_3_4_35);LV_IMAGE_DECLARE(img_3_4_36);
LV_IMAGE_DECLARE(img_3_4_37);LV_IMAGE_DECLARE(img_3_4_38);LV_IMAGE_DECLARE(img_3_4_39);LV_IMAGE_DECLARE(img_3_4_40);
LV_IMAGE_DECLARE(img_3_4_41);LV_IMAGE_DECLARE(img_3_4_42);LV_IMAGE_DECLARE(img_3_4_43);LV_IMAGE_DECLARE(img_3_4_44);
LV_IMAGE_DECLARE(img_3_4_45);LV_IMAGE_DECLARE(img_3_4_46);

//-----------------------------------------------------------------------------------------
// 4-5
LV_IMAGE_DECLARE(img_4_5_01);LV_IMAGE_DECLARE(img_4_5_02);LV_IMAGE_DECLARE(img_4_5_03);LV_IMAGE_DECLARE(img_4_5_04);
LV_IMAGE_DECLARE(img_4_5_05);LV_IMAGE_DECLARE(img_4_5_06);LV_IMAGE_DECLARE(img_4_5_07);LV_IMAGE_DECLARE(img_4_5_08);
LV_IMAGE_DECLARE(img_4_5_09);LV_IMAGE_DECLARE(img_4_5_10);LV_IMAGE_DECLARE(img_4_5_11);LV_IMAGE_DECLARE(img_4_5_12);
LV_IMAGE_DECLARE(img_4_5_13);LV_IMAGE_DECLARE(img_4_5_14);LV_IMAGE_DECLARE(img_4_5_15);LV_IMAGE_DECLARE(img_4_5_16);
LV_IMAGE_DECLARE(img_4_5_17);LV_IMAGE_DECLARE(img_4_5_18);LV_IMAGE_DECLARE(img_4_5_19);LV_IMAGE_DECLARE(img_4_5_20);
LV_IMAGE_DECLARE(img_4_5_21);LV_IMAGE_DECLARE(img_4_5_22);LV_IMAGE_DECLARE(img_4_5_23);LV_IMAGE_DECLARE(img_4_5_24);
LV_IMAGE_DECLARE(img_4_5_25);LV_IMAGE_DECLARE(img_4_5_26);LV_IMAGE_DECLARE(img_4_5_27);LV_IMAGE_DECLARE(img_4_5_28);
LV_IMAGE_DECLARE(img_4_5_29);LV_IMAGE_DECLARE(img_4_5_30);LV_IMAGE_DECLARE(img_4_5_31);LV_IMAGE_DECLARE(img_4_5_32);
LV_IMAGE_DECLARE(img_4_5_33);LV_IMAGE_DECLARE(img_4_5_34);LV_IMAGE_DECLARE(img_4_5_35);LV_IMAGE_DECLARE(img_4_5_36);
LV_IMAGE_DECLARE(img_4_5_37);LV_IMAGE_DECLARE(img_4_5_38);LV_IMAGE_DECLARE(img_4_5_39);LV_IMAGE_DECLARE(img_4_5_40);
LV_IMAGE_DECLARE(img_4_5_41);LV_IMAGE_DECLARE(img_4_5_42);LV_IMAGE_DECLARE(img_4_5_43);LV_IMAGE_DECLARE(img_4_5_44);
LV_IMAGE_DECLARE(img_4_5_45);LV_IMAGE_DECLARE(img_4_5_46);

//-----------------------------------------------------------------------------------------
// 5-6
LV_IMAGE_DECLARE(img_5_6_01);LV_IMAGE_DECLARE(img_5_6_02);LV_IMAGE_DECLARE(img_5_6_03);LV_IMAGE_DECLARE(img_5_6_04);
LV_IMAGE_DECLARE(img_5_6_05);LV_IMAGE_DECLARE(img_5_6_06);LV_IMAGE_DECLARE(img_5_6_07);LV_IMAGE_DECLARE(img_5_6_08);
LV_IMAGE_DECLARE(img_5_6_09);LV_IMAGE_DECLARE(img_5_6_10);LV_IMAGE_DECLARE(img_5_6_11);LV_IMAGE_DECLARE(img_5_6_12);
LV_IMAGE_DECLARE(img_5_6_13);LV_IMAGE_DECLARE(img_5_6_14);LV_IMAGE_DECLARE(img_5_6_15);LV_IMAGE_DECLARE(img_5_6_16);
LV_IMAGE_DECLARE(img_5_6_17);LV_IMAGE_DECLARE(img_5_6_18);LV_IMAGE_DECLARE(img_5_6_19);LV_IMAGE_DECLARE(img_5_6_20);
LV_IMAGE_DECLARE(img_5_6_21);LV_IMAGE_DECLARE(img_5_6_22);LV_IMAGE_DECLARE(img_5_6_23);LV_IMAGE_DECLARE(img_5_6_24);
LV_IMAGE_DECLARE(img_5_6_25);LV_IMAGE_DECLARE(img_5_6_26);LV_IMAGE_DECLARE(img_5_6_27);LV_IMAGE_DECLARE(img_5_6_28);
LV_IMAGE_DECLARE(img_5_6_29);LV_IMAGE_DECLARE(img_5_6_30);LV_IMAGE_DECLARE(img_5_6_31);LV_IMAGE_DECLARE(img_5_6_32);
LV_IMAGE_DECLARE(img_5_6_33);LV_IMAGE_DECLARE(img_5_6_34);LV_IMAGE_DECLARE(img_5_6_35);LV_IMAGE_DECLARE(img_5_6_36);
LV_IMAGE_DECLARE(img_5_6_37);LV_IMAGE_DECLARE(img_5_6_38);LV_IMAGE_DECLARE(img_5_6_39);LV_IMAGE_DECLARE(img_5_6_40);
LV_IMAGE_DECLARE(img_5_6_41);LV_IMAGE_DECLARE(img_5_6_42);LV_IMAGE_DECLARE(img_5_6_43);LV_IMAGE_DECLARE(img_5_6_44);
LV_IMAGE_DECLARE(img_5_6_45);LV_IMAGE_DECLARE(img_5_6_46);

//-----------------------------------------------------------------------------------------
// 6-7
LV_IMAGE_DECLARE(img_6_7_01);LV_IMAGE_DECLARE(img_6_7_02);LV_IMAGE_DECLARE(img_6_7_03);LV_IMAGE_DECLARE(img_6_7_04);
LV_IMAGE_DECLARE(img_6_7_05);LV_IMAGE_DECLARE(img_6_7_06);LV_IMAGE_DECLARE(img_6_7_07);LV_IMAGE_DECLARE(img_6_7_08);
LV_IMAGE_DECLARE(img_6_7_09);LV_IMAGE_DECLARE(img_6_7_10);LV_IMAGE_DECLARE(img_6_7_11);LV_IMAGE_DECLARE(img_6_7_12);
LV_IMAGE_DECLARE(img_6_7_13);LV_IMAGE_DECLARE(img_6_7_14);LV_IMAGE_DECLARE(img_6_7_15);LV_IMAGE_DECLARE(img_6_7_16);
LV_IMAGE_DECLARE(img_6_7_17);LV_IMAGE_DECLARE(img_6_7_18);LV_IMAGE_DECLARE(img_6_7_19);LV_IMAGE_DECLARE(img_6_7_20);
LV_IMAGE_DECLARE(img_6_7_21);LV_IMAGE_DECLARE(img_6_7_22);LV_IMAGE_DECLARE(img_6_7_23);LV_IMAGE_DECLARE(img_6_7_24);
LV_IMAGE_DECLARE(img_6_7_25);LV_IMAGE_DECLARE(img_6_7_26);LV_IMAGE_DECLARE(img_6_7_27);LV_IMAGE_DECLARE(img_6_7_28);
LV_IMAGE_DECLARE(img_6_7_29);LV_IMAGE_DECLARE(img_6_7_30);LV_IMAGE_DECLARE(img_6_7_31);LV_IMAGE_DECLARE(img_6_7_32);
LV_IMAGE_DECLARE(img_6_7_33);LV_IMAGE_DECLARE(img_6_7_34);LV_IMAGE_DECLARE(img_6_7_35);LV_IMAGE_DECLARE(img_6_7_36);
LV_IMAGE_DECLARE(img_6_7_37);LV_IMAGE_DECLARE(img_6_7_38);LV_IMAGE_DECLARE(img_6_7_39);LV_IMAGE_DECLARE(img_6_7_40);
LV_IMAGE_DECLARE(img_6_7_41);LV_IMAGE_DECLARE(img_6_7_42);LV_IMAGE_DECLARE(img_6_7_43);LV_IMAGE_DECLARE(img_6_7_44);
LV_IMAGE_DECLARE(img_6_7_45);LV_IMAGE_DECLARE(img_6_7_46);

//-----------------------------------------------------------------------------------------
// 7-8
LV_IMAGE_DECLARE(img_7_8_01);LV_IMAGE_DECLARE(img_7_8_02);LV_IMAGE_DECLARE(img_7_8_03);LV_IMAGE_DECLARE(img_7_8_04);
LV_IMAGE_DECLARE(img_7_8_05);LV_IMAGE_DECLARE(img_7_8_06);LV_IMAGE_DECLARE(img_7_8_07);LV_IMAGE_DECLARE(img_7_8_08);
LV_IMAGE_DECLARE(img_7_8_09);LV_IMAGE_DECLARE(img_7_8_10);LV_IMAGE_DECLARE(img_7_8_11);LV_IMAGE_DECLARE(img_7_8_12);
LV_IMAGE_DECLARE(img_7_8_13);LV_IMAGE_DECLARE(img_7_8_14);LV_IMAGE_DECLARE(img_7_8_15);LV_IMAGE_DECLARE(img_7_8_16);
LV_IMAGE_DECLARE(img_7_8_17);LV_IMAGE_DECLARE(img_7_8_18);LV_IMAGE_DECLARE(img_7_8_19);LV_IMAGE_DECLARE(img_7_8_20);
LV_IMAGE_DECLARE(img_7_8_21);LV_IMAGE_DECLARE(img_7_8_22);LV_IMAGE_DECLARE(img_7_8_23);LV_IMAGE_DECLARE(img_7_8_24);
LV_IMAGE_DECLARE(img_7_8_25);LV_IMAGE_DECLARE(img_7_8_26);LV_IMAGE_DECLARE(img_7_8_27);LV_IMAGE_DECLARE(img_7_8_28);
LV_IMAGE_DECLARE(img_7_8_29);LV_IMAGE_DECLARE(img_7_8_30);LV_IMAGE_DECLARE(img_7_8_31);LV_IMAGE_DECLARE(img_7_8_32);
LV_IMAGE_DECLARE(img_7_8_33);LV_IMAGE_DECLARE(img_7_8_34);LV_IMAGE_DECLARE(img_7_8_35);LV_IMAGE_DECLARE(img_7_8_36);
LV_IMAGE_DECLARE(img_7_8_37);LV_IMAGE_DECLARE(img_7_8_38);LV_IMAGE_DECLARE(img_7_8_39);LV_IMAGE_DECLARE(img_7_8_40);
LV_IMAGE_DECLARE(img_7_8_41);LV_IMAGE_DECLARE(img_7_8_42);LV_IMAGE_DECLARE(img_7_8_43);LV_IMAGE_DECLARE(img_7_8_44);
LV_IMAGE_DECLARE(img_7_8_45);LV_IMAGE_DECLARE(img_7_8_46);

//-----------------------------------------------------------------------------------------
// 8-9
LV_IMAGE_DECLARE(img_8_9_01);LV_IMAGE_DECLARE(img_8_9_02);LV_IMAGE_DECLARE(img_8_9_03);LV_IMAGE_DECLARE(img_8_9_04);
LV_IMAGE_DECLARE(img_8_9_05);LV_IMAGE_DECLARE(img_8_9_06);LV_IMAGE_DECLARE(img_8_9_07);LV_IMAGE_DECLARE(img_8_9_08);
LV_IMAGE_DECLARE(img_8_9_09);LV_IMAGE_DECLARE(img_8_9_10);LV_IMAGE_DECLARE(img_8_9_11);LV_IMAGE_DECLARE(img_8_9_12);
LV_IMAGE_DECLARE(img_8_9_13);LV_IMAGE_DECLARE(img_8_9_14);LV_IMAGE_DECLARE(img_8_9_15);LV_IMAGE_DECLARE(img_8_9_16);
LV_IMAGE_DECLARE(img_8_9_17);LV_IMAGE_DECLARE(img_8_9_18);LV_IMAGE_DECLARE(img_8_9_19);LV_IMAGE_DECLARE(img_8_9_20);
LV_IMAGE_DECLARE(img_8_9_21);LV_IMAGE_DECLARE(img_8_9_22);LV_IMAGE_DECLARE(img_8_9_23);LV_IMAGE_DECLARE(img_8_9_24);
LV_IMAGE_DECLARE(img_8_9_25);LV_IMAGE_DECLARE(img_8_9_26);LV_IMAGE_DECLARE(img_8_9_27);LV_IMAGE_DECLARE(img_8_9_28);
LV_IMAGE_DECLARE(img_8_9_29);LV_IMAGE_DECLARE(img_8_9_30);LV_IMAGE_DECLARE(img_8_9_31);LV_IMAGE_DECLARE(img_8_9_32);
LV_IMAGE_DECLARE(img_8_9_33);LV_IMAGE_DECLARE(img_8_9_34);LV_IMAGE_DECLARE(img_8_9_35);LV_IMAGE_DECLARE(img_8_9_36);
LV_IMAGE_DECLARE(img_8_9_37);LV_IMAGE_DECLARE(img_8_9_38);LV_IMAGE_DECLARE(img_8_9_39);LV_IMAGE_DECLARE(img_8_9_40);
LV_IMAGE_DECLARE(img_8_9_41);LV_IMAGE_DECLARE(img_8_9_42);LV_IMAGE_DECLARE(img_8_9_43);LV_IMAGE_DECLARE(img_8_9_44);
LV_IMAGE_DECLARE(img_8_9_45);LV_IMAGE_DECLARE(img_8_9_46);

//-----------------------------------------------------------------------------------------
// 9-0
LV_IMAGE_DECLARE(img_9_0_01);LV_IMAGE_DECLARE(img_9_0_02);LV_IMAGE_DECLARE(img_9_0_03);LV_IMAGE_DECLARE(img_9_0_04);
LV_IMAGE_DECLARE(img_9_0_05);LV_IMAGE_DECLARE(img_9_0_06);LV_IMAGE_DECLARE(img_9_0_07);LV_IMAGE_DECLARE(img_9_0_08);
LV_IMAGE_DECLARE(img_9_0_09);LV_IMAGE_DECLARE(img_9_0_10);LV_IMAGE_DECLARE(img_9_0_11);LV_IMAGE_DECLARE(img_9_0_12);
LV_IMAGE_DECLARE(img_9_0_13);LV_IMAGE_DECLARE(img_9_0_14);LV_IMAGE_DECLARE(img_9_0_15);LV_IMAGE_DECLARE(img_9_0_16);
LV_IMAGE_DECLARE(img_9_0_17);LV_IMAGE_DECLARE(img_9_0_18);LV_IMAGE_DECLARE(img_9_0_19);LV_IMAGE_DECLARE(img_9_0_20);
LV_IMAGE_DECLARE(img_9_0_21);LV_IMAGE_DECLARE(img_9_0_22);LV_IMAGE_DECLARE(img_9_0_23);LV_IMAGE_DECLARE(img_9_0_24);
LV_IMAGE_DECLARE(img_9_0_25);LV_IMAGE_DECLARE(img_9_0_26);LV_IMAGE_DECLARE(img_9_0_27);LV_IMAGE_DECLARE(img_9_0_28);
LV_IMAGE_DECLARE(img_9_0_29);LV_IMAGE_DECLARE(img_9_0_30);LV_IMAGE_DECLARE(img_9_0_31);LV_IMAGE_DECLARE(img_9_0_32);
LV_IMAGE_DECLARE(img_9_0_33);LV_IMAGE_DECLARE(img_9_0_34);LV_IMAGE_DECLARE(img_9_0_35);LV_IMAGE_DECLARE(img_9_0_36);
LV_IMAGE_DECLARE(img_9_0_37);LV_IMAGE_DECLARE(img_9_0_38);LV_IMAGE_DECLARE(img_9_0_39);LV_IMAGE_DECLARE(img_9_0_40);
LV_IMAGE_DECLARE(img_9_0_41);LV_IMAGE_DECLARE(img_9_0_42);LV_IMAGE_DECLARE(img_9_0_43);LV_IMAGE_DECLARE(img_9_0_44);
LV_IMAGE_DECLARE(img_9_0_45);LV_IMAGE_DECLARE(img_9_0_46);

//-----------------------------------------------------------------------------------------
static const lv_image_dsc_t * anim_01_imgs[46] = {
    &img_0_1_01, &img_0_1_02, &img_0_1_03, &img_0_1_04,
    &img_0_1_05, &img_0_1_06, &img_0_1_07, &img_0_1_08, &img_0_1_09,
    &img_0_1_10, &img_0_1_11, &img_0_1_12, &img_0_1_13, &img_0_1_14,
    &img_0_1_15, &img_0_1_16, &img_0_1_17, &img_0_1_18, &img_0_1_19,
    &img_0_1_20, &img_0_1_21, &img_0_1_22, &img_0_1_23, &img_0_1_24,
    &img_0_1_25, &img_0_1_26, &img_0_1_27, &img_0_1_28, &img_0_1_29,
    &img_0_1_30, &img_0_1_31, &img_0_1_32, &img_0_1_33, &img_0_1_34,
    &img_0_1_35, &img_0_1_36, &img_0_1_37, &img_0_1_38, &img_0_1_39,
    &img_0_1_40, &img_0_1_41, &img_0_1_42, &img_0_1_43, &img_0_1_44,
    &img_0_1_45, &img_0_1_46
};

//-----------------------------------------------------------------------------------------
static const lv_image_dsc_t * anim_12_imgs[46] = {
    &img_1_2_01, &img_1_2_02, &img_1_2_03, &img_1_2_04,
    &img_1_2_05, &img_1_2_06, &img_1_2_07, &img_1_2_08, &img_1_2_09,
    &img_1_2_10, &img_1_2_11, &img_1_2_12, &img_1_2_13, &img_1_2_14,
    &img_1_2_15, &img_1_2_16, &img_1_2_17, &img_1_2_18, &img_1_2_19,
    &img_1_2_20, &img_1_2_21, &img_1_2_22, &img_1_2_23, &img_1_2_24,
    &img_1_2_25, &img_1_2_26, &img_1_2_27, &img_1_2_28, &img_1_2_29,
    &img_1_2_30, &img_1_2_31, &img_1_2_32, &img_1_2_33, &img_1_2_34,
    &img_1_2_35, &img_1_2_36, &img_1_2_37, &img_1_2_38, &img_1_2_39,
    &img_1_2_40, &img_1_2_41, &img_1_2_42, &img_1_2_43, &img_1_2_44,
    &img_1_2_45, &img_1_2_46
};

//-----------------------------------------------------------------------------------------
static const lv_image_dsc_t * anim_23_imgs[46] = {
    &img_2_3_01, &img_2_3_02, &img_2_3_03, &img_2_3_04,
    &img_2_3_05, &img_2_3_06, &img_2_3_07, &img_2_3_08, &img_2_3_09,
    &img_2_3_10, &img_2_3_11, &img_2_3_12, &img_2_3_13, &img_2_3_14,
    &img_2_3_15, &img_2_3_16, &img_2_3_17, &img_2_3_18, &img_2_3_19,
    &img_2_3_20, &img_2_3_21, &img_2_3_22, &img_2_3_23, &img_2_3_24,
    &img_2_3_25, &img_2_3_26, &img_2_3_27, &img_2_3_28, &img_2_3_29,
    &img_2_3_30, &img_2_3_31, &img_2_3_32, &img_2_3_33, &img_2_3_34,
    &img_2_3_35, &img_2_3_36, &img_2_3_37, &img_2_3_38, &img_2_3_39,
    &img_2_3_40, &img_2_3_41, &img_2_3_42, &img_2_3_43, &img_2_3_44,
    &img_2_3_45, &img_2_3_46
};

//-----------------------------------------------------------------------------------------
static const lv_image_dsc_t * anim_34_imgs[46] = {
    &img_3_4_01, &img_3_4_02, &img_3_4_03, &img_3_4_04,
    &img_3_4_05, &img_3_4_06, &img_3_4_07, &img_3_4_08, &img_3_4_09,
    &img_3_4_10, &img_3_4_11, &img_3_4_12, &img_3_4_13, &img_3_4_14,
    &img_3_4_15, &img_3_4_16, &img_3_4_17, &img_3_4_18, &img_3_4_19,
    &img_3_4_20, &img_3_4_21, &img_3_4_22, &img_3_4_23, &img_3_4_24,
    &img_3_4_25, &img_3_4_26, &img_3_4_27, &img_3_4_28, &img_3_4_29,
    &img_3_4_30, &img_3_4_31, &img_3_4_32, &img_3_4_33, &img_3_4_34,
    &img_3_4_35, &img_3_4_36, &img_3_4_37, &img_3_4_38, &img_3_4_39,
    &img_3_4_40, &img_3_4_41, &img_3_4_42, &img_3_4_43, &img_3_4_44,
    &img_3_4_45, &img_3_4_46
};

//-----------------------------------------------------------------------------------------
static const lv_image_dsc_t * anim_45_imgs[46] = {
    &img_4_5_01, &img_4_5_02, &img_4_5_03, &img_4_5_04,
    &img_4_5_05, &img_4_5_06, &img_4_5_07, &img_4_5_08, &img_4_5_09,
    &img_4_5_10, &img_4_5_11, &img_4_5_12, &img_4_5_13, &img_4_5_14,
    &img_4_5_15, &img_4_5_16, &img_4_5_17, &img_4_5_18, &img_4_5_19,
    &img_4_5_20, &img_4_5_21, &img_4_5_22, &img_4_5_23, &img_4_5_24,
    &img_4_5_25, &img_4_5_26, &img_4_5_27, &img_4_5_28, &img_4_5_29,
    &img_4_5_30, &img_4_5_31, &img_4_5_32, &img_4_5_33, &img_4_5_34,
    &img_4_5_35, &img_4_5_36, &img_4_5_37, &img_4_5_38, &img_4_5_39,
    &img_4_5_40, &img_4_5_41, &img_4_5_42, &img_4_5_43, &img_4_5_44,
    &img_4_5_45, &img_4_5_46
};

//-----------------------------------------------------------------------------------------
static const lv_image_dsc_t * anim_56_imgs[46] = {
    &img_5_6_01, &img_5_6_02, &img_5_6_03, &img_5_6_04,
    &img_5_6_05, &img_5_6_06, &img_5_6_07, &img_5_6_08, &img_5_6_09,
    &img_5_6_10, &img_5_6_11, &img_5_6_12, &img_5_6_13, &img_5_6_14,
    &img_5_6_15, &img_5_6_16, &img_5_6_17, &img_5_6_18, &img_5_6_19,
    &img_5_6_20, &img_5_6_21, &img_5_6_22, &img_5_6_23, &img_5_6_24,
    &img_5_6_25, &img_5_6_26, &img_5_6_27, &img_5_6_28, &img_5_6_29,
    &img_5_6_30, &img_5_6_31, &img_5_6_32, &img_5_6_33, &img_5_6_34,
    &img_5_6_35, &img_5_6_36, &img_5_6_37, &img_5_6_38, &img_5_6_39,
    &img_5_6_40, &img_5_6_41, &img_5_6_42, &img_5_6_43, &img_5_6_44,
    &img_5_6_45, &img_5_6_46
};

//-----------------------------------------------------------------------------------------
static const lv_image_dsc_t * anim_67_imgs[46] = {
    &img_6_7_01, &img_6_7_02, &img_6_7_03, &img_6_7_04,
    &img_6_7_05, &img_6_7_06, &img_6_7_07, &img_6_7_08, &img_6_7_09,
    &img_6_7_10, &img_6_7_11, &img_6_7_12, &img_6_7_13, &img_6_7_14,
    &img_6_7_15, &img_6_7_16, &img_6_7_17, &img_6_7_18, &img_6_7_19,
    &img_6_7_20, &img_6_7_21, &img_6_7_22, &img_6_7_23, &img_6_7_24,
    &img_6_7_25, &img_6_7_26, &img_6_7_27, &img_6_7_28, &img_6_7_29,
    &img_6_7_30, &img_6_7_31, &img_6_7_32, &img_6_7_33, &img_6_7_34,
    &img_6_7_35, &img_6_7_36, &img_6_7_37, &img_6_7_38, &img_6_7_39,
    &img_6_7_40, &img_6_7_41, &img_6_7_42, &img_6_7_43, &img_6_7_44,
    &img_6_7_45, &img_6_7_46
};

//-----------------------------------------------------------------------------------------
static const lv_image_dsc_t * anim_78_imgs[46] = {
    &img_7_8_01, &img_7_8_02, &img_7_8_03, &img_7_8_04,
    &img_7_8_05, &img_7_8_06, &img_7_8_07, &img_7_8_08, &img_7_8_09,
    &img_7_8_10, &img_7_8_11, &img_7_8_12, &img_7_8_13, &img_7_8_14,
    &img_7_8_15, &img_7_8_16, &img_7_8_17, &img_7_8_18, &img_7_8_19,
    &img_7_8_20, &img_7_8_21, &img_7_8_22, &img_7_8_23, &img_7_8_24,
    &img_7_8_25, &img_7_8_26, &img_7_8_27, &img_7_8_28, &img_7_8_29,
    &img_7_8_30, &img_7_8_31, &img_7_8_32, &img_7_8_33, &img_7_8_34,
    &img_7_8_35, &img_7_8_36, &img_7_8_37, &img_7_8_38, &img_7_8_39,
    &img_7_8_40, &img_7_8_41, &img_7_8_42, &img_7_8_43, &img_7_8_44,
    &img_7_8_45, &img_7_8_46
};

//-----------------------------------------------------------------------------------------
static const lv_image_dsc_t * anim_89_imgs[46] = {
    &img_8_9_01, &img_8_9_02, &img_8_9_03, &img_8_9_04,
    &img_8_9_05, &img_8_9_06, &img_8_9_07, &img_8_9_08, &img_8_9_09,
    &img_8_9_10, &img_8_9_11, &img_8_9_12, &img_8_9_13, &img_8_9_14,
    &img_8_9_15, &img_8_9_16, &img_8_9_17, &img_8_9_18, &img_8_9_19,
    &img_8_9_20, &img_8_9_21, &img_8_9_22, &img_8_9_23, &img_8_9_24,
    &img_8_9_25, &img_8_9_26, &img_8_9_27, &img_8_9_28, &img_8_9_29,
    &img_8_9_30, &img_8_9_31, &img_8_9_32, &img_8_9_33, &img_8_9_34,
    &img_8_9_35, &img_8_9_36, &img_8_9_37, &img_8_9_38, &img_8_9_39,
    &img_8_9_40, &img_8_9_41, &img_8_9_42, &img_8_9_43, &img_8_9_44,
    &img_8_9_45, &img_8_9_46
};

//-----------------------------------------------------------------------------------------
static const lv_image_dsc_t * anim_90_imgs[46] = {
    &img_9_0_01, &img_9_0_02, &img_9_0_03, &img_9_0_04,
    &img_9_0_05, &img_9_0_06, &img_9_0_07, &img_9_0_08, &img_9_0_09,
    &img_9_0_10, &img_9_0_11, &img_9_0_12, &img_9_0_13, &img_9_0_14,
    &img_9_0_15, &img_9_0_16, &img_9_0_17, &img_9_0_18, &img_9_0_19,
    &img_9_0_20, &img_9_0_21, &img_9_0_22, &img_9_0_23, &img_9_0_24,
    &img_9_0_25, &img_9_0_26, &img_9_0_27, &img_9_0_28, &img_9_0_29,
    &img_9_0_30, &img_9_0_31, &img_9_0_32, &img_9_0_33, &img_9_0_34,
    &img_9_0_35, &img_9_0_36, &img_9_0_37, &img_9_0_38, &img_9_0_39,
    &img_9_0_40, &img_9_0_41, &img_9_0_42, &img_9_0_43, &img_9_0_44,
    &img_9_0_45, &img_9_0_46
};

//-----------------------------------------------------------------------------------------
#if VW_DEMO_SEQUENTIAL_ANIMATION

static const lv_image_dsc_t ** all_anims_array[10] = {
    (const lv_image_dsc_t **)anim_01_imgs,
    (const lv_image_dsc_t **)anim_12_imgs,
    (const lv_image_dsc_t **)anim_23_imgs,
    (const lv_image_dsc_t **)anim_34_imgs,
    (const lv_image_dsc_t **)anim_45_imgs,
    (const lv_image_dsc_t **)anim_56_imgs,
    (const lv_image_dsc_t **)anim_67_imgs,
    (const lv_image_dsc_t **)anim_78_imgs,
    (const lv_image_dsc_t **)anim_89_imgs,
    (const lv_image_dsc_t **)anim_90_imgs
};

static lv_obj_t * animimg;         // Объект картинки-анимации
static uint8_t current_step = 0;   // Текущая цифра (от 0 до 9)

static void timer_xcb(lv_timer_t * timer)
{
    // Загружаем нужный массив кадров (в зависимости от текущего шага)
    lv_animimg_set_src(animimg, (const void **)all_anims_array[current_step], 46);
    
    // Запускаем анимацию
    lv_animimg_start(animimg);

    // Увеличиваем счетчик для следующего нажатия
    current_step++;
    if(current_step >= 10) {
        current_step = 0; // Возвращаемся к 0 после 9
    }
}

#endif // VW_DEMO_SEQUENTIAL_ANIMATION

//-----------------------------------------------------------------------------------------
void lvgl_lcd()
{
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_pad_all( lv_screen_active(), 0, LV_PART_MAIN );

    #if VW_DEMO_SEQUENTIAL_ANIMATION

    // Создаем объект анимации и выводим его по центру
    animimg = lv_animimg_create(lv_screen_active());
    lv_obj_center(animimg);

    // Устанавливаем начальные параметры (покажем просто первый кадр "0", не запуская)
    lv_animimg_set_src(animimg, (const void **)all_anims_array[0], 46);
    lv_animimg_set_duration(animimg, 1500);   
    lv_animimg_set_repeat_count(animimg, 1);

    lv_obj_set_style_image_recolor(animimg, lv_color_hex(0xFEB563), LV_PART_MAIN);
    lv_obj_set_style_image_recolor_opa(animimg, 255, LV_PART_MAIN);

    lv_timer_t* timer = lv_timer_create( timer_xcb, 3000, 0 );
#endif // VW_DEMO_SEQUENTIAL_ANIMATION

#if VW_DEMO_WATCHFACE
    lv_obj_t * animimg0 = lv_animimg_create(lv_screen_active());
    lv_obj_align(animimg0, LV_ALIGN_TOP_LEFT, 7, 27);
    lv_animimg_set_src(animimg0, (const void **) anim_01_imgs, 46);
    lv_animimg_set_duration(animimg0, 1500);
    lv_animimg_set_repeat_count(animimg0, LV_ANIM_REPEAT_INFINITE);
    lv_animimg_start(animimg0);

    lv_obj_t * animimg1 = lv_animimg_create(lv_screen_active());
    lv_obj_align(animimg1, LV_ALIGN_TOP_RIGHT, -7, 27);
    lv_animimg_set_src(animimg1, (const void **) anim_12_imgs, 46);
    lv_animimg_set_duration(animimg1, 1500);
    lv_animimg_set_repeat_count(animimg1, LV_ANIM_REPEAT_INFINITE);
    lv_animimg_start(animimg1);
    
    lv_obj_t * animimg2 = lv_animimg_create(lv_screen_active());
    lv_obj_align(animimg2, LV_ALIGN_BOTTOM_LEFT, 7, -27);
    lv_animimg_set_src(animimg2, (const void **) anim_23_imgs, 46);
    lv_animimg_set_duration(animimg2, 1500);
    lv_animimg_set_repeat_count(animimg2, LV_ANIM_REPEAT_INFINITE);
    lv_animimg_start(animimg2);

    lv_obj_t * animimg3 = lv_animimg_create(lv_screen_active());
    lv_obj_align(animimg3, LV_ALIGN_BOTTOM_RIGHT, -7, -27);
    lv_animimg_set_src(animimg3, (const void **) anim_34_imgs, 46);
    lv_animimg_set_duration(animimg3, 1500);
    lv_animimg_set_repeat_count(animimg3, LV_ANIM_REPEAT_INFINITE);
    lv_animimg_start(animimg3);

    #endif // VW_DEMO_WATCHFACE
}

//-----------------------------------------------------------------------------------------

#endif // MOD_LVGL_LCD