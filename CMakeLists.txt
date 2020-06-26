file(GLOB_RECURSE SOURCES tkos.c views/*.c views/*/*.c hmi/*/*.c)

idf_component_register(SRCS ${SOURCES}
                       INCLUDE_DIRS "."
                       REQUIRES lvgl_esp32_drivers lvgl lv_examples lvgl_tft lvgl_touch)