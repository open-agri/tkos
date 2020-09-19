/**
 * @file menu.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Custom menus for encoder.
 * @version 0.1
 * @date 2020-09-17
 * 
 * 
 */

#pragma once

#include "lvgl/lvgl.h"

#define TK_MENU_MAX_ITEMS   30

typedef enum
{
    TK_MENU_ITEM_BUTTON, // Not implemented
    TK_MENU_ITEM_SWITCH,
    TK_MENU_ITEM_SLIDER,
    TK_MENU_ITEM_DDLIST, // Not implemented
    TK_MENU_ITEM_LABEL,  // Not implemented
    TK_MENU_ITEM_SPACER  // Not implemented
} tk_menu_item_type_t;

typedef enum
{
    TK_MENU_BINDING_INT,
    TK_MENU_BINDING_UINT,
    TK_MENU_BINDING_DOUBLE
} tk_menu_binding_type_t;

typedef struct tk_menu_item
{
    /**
     * @brief The type of widget represented.
     *
     */
    tk_menu_item_type_t type;

    /**
     * @brief Whether this item should be skipped and grayed out.
     *
     */
    bool disabled;

    /**
     * @brief The type of the binding.
     * 
     */
    tk_menu_binding_type_t binding_type;

    /**
     * @brief Pointer to the bound variable.
     * 
     */
    void *binding;

    /**
     * @brief The minimum value.
     * 
     */
    double binding_min;

    /**
     * @brief The maximum value.
     * 
     */
    double binding_max;

    /**
     * @brief The number of steps in a slider.
     * 
     */
    int binding_steps;

    /**
     * @brief Description of the item.
     * 
     */
    char *desc;

    /**
     * @brief Called when a button is selected in a list or its value changed (for other widgets).
     * 
     */
    void (*value_change_cb)(struct tk_menu_item *sender);

    // Widgets
    lv_obj_t *base;
    lv_obj_t *label;
    lv_obj_t *control;

} tk_menu_item_t;

typedef struct
{
    int items_count;
    tk_menu_item_t *items[TK_MENU_MAX_ITEMS];
    lv_group_t *group;
    void (*focus_change_cb)(tk_menu_item_t *focused);
} tk_menu_t;

#define TK_MENU_VALUE_CHANGE_CB_DECLARE(name) static void name(tk_menu_item_t *sender)
#define TK_MENU_FOCUS_CHANGE_CB_DECLARE(name) static void name(tk_menu_item_t *focused)

lv_obj_t *tk_menu_create(lv_obj_t *parent, lv_group_t *group, tk_menu_t *menu);
tk_menu_item_t *tk_menu_get_current_item(lv_group_t *group);
// void tk_menu_add_item(tk_menu_t *menu, tk_menu_item_t *item);
