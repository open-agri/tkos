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
#define LIGHT_DARK      3030
#define LIGHT_BRIGHT    2950
#define THEME_THRESHOLD_LOW     0.15
#define THEME_THRESHOLD_HIGH    0.25

/**
 * @brief Sets whether the brightness should vary based on current brightness.
 * 
 * @param is_auto Sets the automatic mode to on or off.
 */
void hmi_brightness_set_auto(bool is_auto);

/**
 * @brief Sets the desired brightness level and switches to manual mode.
 * 
 * @param value Brightness value (0-1).
 */
void hmi_brightness_set_value(double value);

/**
 * @brief Gets the current automatic contfiguration.
 * 
 * @return true Automatic mode.
 * @return false Manual mode.
 */
bool hmi_brightness_get_auto(void);

/**
 * @brief Gets the current brightness value, even in automatic mode.
 * 
 * @return double The display brightness (0-1).
 */
double hmi_brightness_get_value(void);

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
