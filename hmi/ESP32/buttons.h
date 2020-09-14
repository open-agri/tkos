/**
 * @file buttons.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Driver for pushbuttons.
 * @version 0.1
 * @date 2020-09-10
 * 
 * 
 */

#pragma once

#include "lvgl/lvgl.h"
#include "driver/gpio.h"

#define HMI_BUTTON_PIN_LEFT 16
#define HMI_BUTTON_PIN_RIGHT 17

#define HMI_BUTTON_ID_LEFT 1
#define HMI_BUTTON_ID_RIGHT 0

// Debounce time
#define HMI_BUTTON_DEB_US 15000
// Delay of sampling after interrupt
#define HMI_BUTTON_DEL_US 10000


/**
 * @brief Buttons driver initialization function.
 * 
 */
void hmi_buttons_init();

/**
 * @brief The callback function for interfacing this driver with lvgl.
 * 
 * @param drv The indev driver.
 * @param data The output data for the driver.
 * @return true Never.
 * @return false Always.
 */
bool hmi_buttons_read(lv_indev_drv_t *drv, lv_indev_data_t *data);

