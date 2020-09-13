/**
 * @file tk_view.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief The view structure.
 * @version 0.1
 * @date 2020-09-11
 * 
 * 
 */

#ifndef TK_VIEW_H
#define TK_VIEW_H

#include "lvgl/lvgl.h"
#include "bars.h"

/**
 * @brief Represents a view with its generated content and its contextualized bottom bar.
 * 
 */
typedef struct {
    lv_obj_t *content;
    tk_bottom_bar_configuration_t bottom_bar_configuration;
} tk_view_t;

#endif //TK_VIEW_H
