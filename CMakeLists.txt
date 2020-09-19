file(GLOB_RECURSE SOURCES tkos.c ui/*.c ui/*/*.c ui/views/*/*.c hmi/*/*.c model/*/*.c ble/ble.c ble/*/*.c)
set(INCLUDES .)

idf_component_register(SRCS ${SOURCES}
                       INCLUDE_DIRS ${INCLUDES}
                       REQUIRES lvgl_esp32_drivers lvgl lvgl_tft lvgl_touch nvs_flash bt)