/**
 * @file brightness.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Screen brightness management.
 * @version 0.1
 * @date 2020-09-13
 * 
 * 
 */

#pragma once

#include "model/brightness.h"

#define BRIGHTNESS_PIN  27
#define BRIGHTNESS_MIN  25
#define BRIGHTNESS_MAX  1023
#define LIGHT_DARK      4095
#define LIGHT_BRIGHT    1800
#define THEME_THRESHOLD_LOW     0.10
#define THEME_THRESHOLD_HIGH    0.15

/**
 * @brief Initializes the brightness manager.
 * 
 * @param settings Initial settings.
 */
void hmi_brightness_init(tk_brightness_settings_t *settings);

/**
 * @brief Switches the main theme.
 * 
 * @param light true for material light, false for material dark.
 */
void switch_theme(bool light);

/**
 * @brief Reads data from the local settings and pushes it to the hardware. Saves updated value to settings.
 * 
 */
void brightness_task(lv_task_t *task);

// TODO: Automatic theme switch.
