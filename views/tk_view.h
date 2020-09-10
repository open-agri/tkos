//
// Created by riccardo on 26/06/20.
//

#ifndef TK_VIEW_H
#define TK_VIEW_H

#include "lvgl/lvgl.h"
#include "bars.h"

typedef struct {
    lv_obj_t *content;
    tk_bottom_bar_configuration_t bottom_bar_configuration;
} tk_view_t;

#endif //TK_VIEW_H
