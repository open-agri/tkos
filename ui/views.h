/**
 * @file views.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief TKOS views.
 * @version 0.1
 * @date 2020-09-11
 * 
 * 
 */

#pragma once

#include <stdlib.h>
#include "lvgl/lvgl.h"
#include "tkos.h"
#include "ui/bars/bars.h"

// Views
#include "ui/views/main/main_view.h"
#include "ui/views/menu/menu_view.h"
#include "ui/views/brightness/brightness_view.h"

// Last: override colors
#include "ui/styles/tk_style.h"

typedef tk_view_t (*tk_view_generator)(void);

lv_obj_t *current_view_content;

/**
 * @brief Navigates to the view defined by its generator, and optionally adds it to the navigation stack.
 * 
 * @param generator The generator for the new view.
 * @param record_stack Record this navigation in the stack if set to true.
 */
void view_navigate(tk_view_generator generator, bool record_stack);

/**
 * @brief Pops an item from the navigation stack, generates its view and shows its content on screen.
 * 
 */
void view_navigate_back();

