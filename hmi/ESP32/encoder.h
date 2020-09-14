/**
 * @file encoder.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Driver for rotary encoder.
 * @version 0.1
 * @date 2020-09-10
 * 
 * 
 */

#pragma once

#include "lvgl/lvgl.h"
#include "driver/gpio.h"

#define HMI_ENCODER_PIN_A 26
#define HMI_ENCODER_PIN_B 25

#define HMI_ENCODER_INVERT 0
#define HMI_ENCODER_DEB_US 500 // Debounce time
#define HMI_ENCODER_DEL_US 1000   // Sampling delay after interrupt

/**
 * @brief Encoder driver initialization function.
 * 
 */
void hmi_encoder_init();

/**
 * @brief The callback function for interfacing this driver with lvgl.
 * 
 * @param drv The indev driver.
 * @param data The output data for the driver.
 * @return true Never.
 * @return false Always.
 */
bool hmi_encoder_read(lv_indev_drv_t *drv, lv_indev_data_t *data);

