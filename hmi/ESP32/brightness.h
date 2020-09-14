/**
 * @file brightness.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Screen brightness management.
 * @version 0.1
 * @date 2020-09-13
 * 
 * 
 */

#include "model/brightness.h"

/**
 * @brief Sets whether the brightness should vary based on current brightness.
 * 
 * @param is_auto Sets the automatic mode to on or off.
 */
void hmi_brightness_set_auto(bool is_auto);

/**
 * @brief Sets the desired brightness level and switches to manual mode.
 * 
 * @param value Brightness value (0-255).
 */
void hmi_brightness_set_value(unsigned short int value);

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
 * @return unsigned short int The display brightness (0-255).
 */
unsigned short int hmi_brightness_get_value(void);

/**
 * @brief Initializes the brightness manager.
 * 
 */
void hmi_brightness_init(tk_brightness_settings_t settings);

// TODO: Automatic theme switch.
