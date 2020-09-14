/**
 * @file bars.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Generator for top and bottom bars.
 * @version 0.1
 * @date 2020-09-11
 * 
 * 
 */

#pragma once

#include <stdbool.h>
#include "lvgl/lvgl.h"

typedef void (*tk_void_callback)(void);

/**
 * @brief Represents an item of the context menu of a bar button.
 * 
 */
typedef struct
{
    char *text;
    tk_void_callback click_callback;
} tk_menu_item_t;

/**
 * @brief Represents a bar button and its set of menu items.
 * 
 */
typedef struct
{
    char *text;
    tk_void_callback click_callback;
    int items_count;
    tk_menu_item_t menu[10];
} tk_bottom_bar_button_t;

/**
 * @brief Represents the entire bottom bar.
 * 
 */
typedef struct
{
    tk_bottom_bar_button_t left_button;
    char *center_text;
    tk_bottom_bar_button_t right_button;
} tk_bottom_bar_configuration_t;

/**
 * @brief Represents the entire top bar.
 * 
 */
typedef struct
{
    char *title;
} tk_top_bar_configuration_t;

/**
 * @brief The bottom bar generator.
 * 
 * @param configuration The configuration to use for generating the bottom bar.
 * @param original Set to true to save this configuration to `original_bottom_bar_configuration`.
 * @return lv_obj_t* The generated bottom bar.
 */
lv_obj_t *build_bottom_bar(tk_bottom_bar_configuration_t configuration, bool original);

/**
 * @brief The top bar generator.
 * 
 * @param configuration The configuration to use for generating the top bar.
 * @return lv_obj_t* The generated top bar.
 */
lv_obj_t *build_top_bar(tk_top_bar_configuration_t configuration);

