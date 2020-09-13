/**
 * @file refresh.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Global refresh signal source.
 * @version 0.1
 * @date 2020-09-11
 * 
 * 
 */

#ifndef REFRESH_H
#define REFRESH_H

#include "lvgl.h"

// TODO: NOOOOOHHHHHH HHHH H h
lv_obj_t *tk_top_bar;

/**
 * @brief This is an lvgl task which is the source of the global refresh signal.
 * 
 * @param task Declared because lvgl tasks need this.
 */
void tk_top_bar_refresher_task(lv_task_t *task);

#endif