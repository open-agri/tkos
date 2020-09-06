#ifndef ENCODER_H
#define ENCODER_H

#include "lvgl/lvgl.h"
#include "driver/gpio.h"

#define HMI_ENCODER_PIN_A 25
#define HMI_ENCODER_PIN_B 26

#define HMI_ENCODER_INVERT 0
#define HMI_ENCODER_DEB_US 15000 // Debounce time
#define HMI_ENCODER_DEL_US 500   // Sampling delay after interrupt

void hmi_encoder_init();
bool hmi_encoder_read(lv_indev_drv_t *drv, lv_indev_data_t *data);

#endif