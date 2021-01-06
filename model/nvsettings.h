/**
 * @file nvsettings.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Non-volatile settings system
 * @version 0.1
 * @date 2020-11-26
 *
 *
 */

#pragma once

#include <stdbool.h>

void nv_init();
void nv_load_settings();
void nv_load_apply_settings();

// SETTINGS
void nv_set_brightness_auto(bool automatic);
bool nv_get_brightness_auto();

void nv_set_brightness_man_level(double level);
double nv_get_brightness_man_level();
