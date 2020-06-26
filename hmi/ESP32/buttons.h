#ifndef BUTTONS_H
#define BUTTONS_H

#include "lvgl/lvgl.h"
#include "driver/gpio.h"

#define HMI_BUTTON_PIN_LEFT 32
#define HMI_BUTTON_PIN_RIGHT 33

#define HMI_BUTTON_ID_LEFT 1
#define HMI_BUTTON_ID_RIGHT 0

#define HMI_BUTTON_DEB_US 15000 // Debounce time
#define HMI_BUTTON_DEL_US 10000   // Sampling delay after interrupt

void hmi_buttons_init();
bool hmi_buttons_read(lv_indev_drv_t *drv, lv_indev_data_t *data);

#endif