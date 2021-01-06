/**
 * @file theme.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Theme settings model.
 * @version 0.1
 * @date 2020-09-15
 * 
 * 
 */

#include "lvgl/lvgl.h"

typedef struct {
    bool automatic;
    bool light;
    lv_color_t primary_color;
    lv_color_t secondary_color;
} tk_theme_settings_t;