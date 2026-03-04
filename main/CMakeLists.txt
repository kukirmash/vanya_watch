file(GLOB image_frames "project_lcd/images/andrew_watchface/*.c")

idf_component_register(
    SRCS 
        "main.c" 
        "project_lcd/project_lcd_main.c"
        ${image_frames}
    INCLUDE_DIRS 
        "." 
        "project_lcd"
)