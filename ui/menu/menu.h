/**
 * @file menu.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Custom menus for encoder.
 * @version 0.1
 * @date 2020-09-17
 * 
 * 
 */

#include "lvgl/lvgl.h"

#pragma once

typedef enum
{
    TK_MENU_ITEM_BUTTON,
    TK_MENU_ITEM_SWITCH,
    TK_MENU_ITEM_SLIDER,
    TK_MENU_ITEM_DDLIST,
    TK_MENU_ITEM_LABEL,
    TK_MENU_ITEM_SPACER
} tk_menu_item_type_t;

typedef enum
{
    TK_MENU_BINDING_INT,
    TK_MENU_BINDING_UINT,
    TK_MENU_BINDING_DOUBLE
} tk_menu_binding_type_t;

typedef struct
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
    bool enabled;

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
    int binding_min;

    /**
     * @brief The maximum value.
     * 
     */
    int binding_max;

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

    // Widgets
    lv_obj_t *base;
    lv_obj_t *label;
    lv_obj_t *control;

} tk_menu_item_t;

typedef struct
{
    int items_count;
    tk_menu_item_t *items;
    lv_group_t *group;
} tk_menu_t;

lv_obj_t *tk_menu_create(lv_obj_t *parent, lv_group_t *group, tk_menu_t *menu);
tk_menu_item_t *tk_menu_get_current_item(lv_group_t *group);
void tk_menu_add_item(tk_menu_t *menu, tk_menu_item_t *item);
