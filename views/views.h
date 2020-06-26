#ifndef VIEWS_H
#define VIEWS_H

#include <stdlib.h>
#include "lvgl/lvgl.h"
#include "bars.h"

// Views
#include "main/main_view.h"
#include "settings/settings_info_view.h"

void view_navigate(tk_view *(*view_generator)(void), bool record_stack);
void view_navigate_back();

#endif