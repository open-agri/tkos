#ifndef REFRESH_H
#define REFRESH_H

#include "lvgl.h"

lv_obj_t *tk_top_bar;

void tk_top_bar_refresher_task(lv_task_t *task);

#endif