#ifndef BARS_H
#define BARS_H

#include <stdbool.h>
#include "lvgl/lvgl.h"

typedef void *(*tk_void_callback)(void);

typedef struct tk_menu_item
{
    char *text;
    tk_void_callback click_callback;
} tk_menu_item;

typedef struct
{
    bool visible;
    char *text;
    tk_void_callback click_callback;
    tk_menu_item *menu;

} tk_bottom_bar_button;

typedef struct
{
    tk_bottom_bar_button left_button;
    char *center_symbol;
    tk_bottom_bar_button right_button;
} tk_bottom_bar_configuration;

lv_obj_t *build_bottom_bar(tk_bottom_bar_configuration *configuration);

#endif