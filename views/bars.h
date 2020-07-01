#ifndef BARS_H
#define BARS_H

#include <stdbool.h>
#include "lvgl/lvgl.h"

typedef void (*tk_void_callback)(void);

// TODO: Move to dynamic allocation

typedef struct tk_menu_item
{
    char text[30];
    tk_void_callback click_callback;
} tk_menu_item;

typedef struct
{
    char text[30];
    tk_void_callback click_callback;
    int items_count;
    tk_menu_item menu[10];
} tk_bottom_bar_button;

typedef struct
{
    tk_bottom_bar_button left_button;
    char center_symbol[16];
    tk_bottom_bar_button right_button;
} tk_bottom_bar_configuration;

lv_obj_t *build_bottom_bar(tk_bottom_bar_configuration configuration, bool original);

#endif