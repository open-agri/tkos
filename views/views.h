#ifndef VIEWS_H
#define VIEWS_H

#include <stdlib.h>
#include "lvgl/lvgl.h"
#include "bars.h"

// Views
#include "main/main_view.h"
// TODO: Add screen
// #include "settings/settings_info_view.h"

typedef tk_view (*tk_view_generator)(void);

void view_navigate(tk_view_generator generator, bool record_stack);
void view_navigate_back();

#endif