file(GLOB image_frames "project_lcd/images/watchface2/*.c")

idf_component_register(
    SRCS 
        "main.c" 
        "project_lcd/control_center.c"
        "project_lcd/demo_sequential.c"
        "project_lcd/project_lcd_main.c"
        "project_lcd/watchface.c"
        ${image_frames}
    INCLUDE_DIRS 
        "." 
        "project_lcd"
)

target_compile_definitions(${COMPONENT_LIB} PRIVATE LV_LVGL_H_INCLUDE_SIMPLE)