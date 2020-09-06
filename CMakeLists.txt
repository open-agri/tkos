file(GLOB_RECURSE SOURCES tkos.c views/*.c views/*/*.c hmi/*/*.c)
set(INCLUDES . views/ hmi/)

idf_component_register(SRCS ${SOURCES}
                       INCLUDE_DIRS ${INCLUDES}
                       REQUIRES lvgl_esp32_drivers lvgl lvgl_tft lvgl_touch)