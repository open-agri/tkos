#ifndef VIEWS_H
#define VIEWS_H

#include <stdlib.h>
#include "lvgl/lvgl.h"
#include "bars.h"
#include "tkos.h"

// Views
#include "main/main_view.h"
#include "menu/menu_view.h"

#include "styles/tk_style.h"

typedef tk_view_t (*tk_view_generator)(void);

lv_obj_t *current_view_content;

void view_navigate(tk_view_generator generator, bool record_stack);
void view_navigate_back();

#endif