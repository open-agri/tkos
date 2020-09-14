/**
 * @file brightness.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Brightness settings model.
 * @version 0.1
 * @date 2020-09-14
 * 
 * 
 */

#pragma once

typedef struct {
    bool automatic;
    unsigned short int level;
} tk_brightness_settings_t;