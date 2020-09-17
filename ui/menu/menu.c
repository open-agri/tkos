/**
 * @file menu.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Custom menus for encoder.
 * @version 0.1
 * @date 2020-09-17
 * 
 * 
 */

#include "menu.h"

#include "ui/views.h"

#include "esp_log.h"

#define TAG "Menu"

// The page widget
lv_obj_t *menu_widget;

LV_EVENT_CB_DECLARE(item_base_event_cb)
{
    // Key
    if (e == LV_EVENT_KEY)
    {
        // Exit with no user data
        if (obj->user_data == NULL)
            return;

        tk_menu_item_t item = *(tk_menu_item_t *)obj->user_data;

        // If object is base and a control exists, send the key to the control
        if (obj == item.base && item.control != NULL)
        {
            lv_obj_set_state(item.control, LV_STATE_EDITED);
            lv_event_send(item.control, LV_EVENT_KEY, lv_event_get_data());
            lv_obj_set_state(item.control, LV_STATE_DEFAULT);
        }
    }
    // Focus (toggle)
    else if (e == LV_EVENT_FOCUSED)
    {
        if (obj->user_data == NULL)
            return;

        tk_menu_item_t item = *(tk_menu_item_t *)obj->user_data;
        tk_menu_t menu = *(tk_menu_t *)menu_widget->user_data;

        // Toggle switch
        if (item.type == TK_MENU_ITEM_SWITCH && item.control != NULL && lv_group_get_editing(menu.group))
        {
            lv_group_set_editing(menu.group, false);
            lv_event_send(item.control, LV_EVENT_VALUE_CHANGED, NULL);
        }
    }
}

LV_EVENT_CB_DECLARE(item_control_event_cb)
{
    // Check for user data, binding and control
    if (obj->user_data == NULL)
        return;
    tk_menu_item_t *item = (tk_menu_item_t *)obj->user_data;

    if (item->binding == NULL)
        return;

    if (item->control == NULL)
        return;

    // Refresh
    if (e == LV_EVENT_REFRESH)
    {
        switch (item->type)
        {
        case TK_MENU_ITEM_SWITCH:
            *(int *)item->binding ? lv_switch_on(item->control, LV_ANIM_ON) : lv_switch_off(item->control, LV_ANIM_ON);
            break;
        default:
            break;
        }
    }
    // Value change
    else if (e == LV_EVENT_VALUE_CHANGED)
    {
        ESP_LOGI(TAG, "%s item value changed.", item->desc);
        switch (item->type)
        {
        case TK_MENU_ITEM_SWITCH:
            (*(bool *)item->binding) = !(*(bool *)item->binding);
            ESP_LOGI(TAG, "%s switch binding set to %d.", item->desc, *(int *)item->binding);
            break;
        default:
            break;
        }
    }
}

void group_focus_cb(lv_group_t *g)
{
    //if (tk_menu_get_current_item(g)->enabled == false)
    //lv_group_focus_next(g); AND PREVIOUS

    lv_page_focus(menu_widget, lv_group_get_focused(g), true);

    // External callback
}

lv_obj_t *tk_menu_create(lv_obj_t *parent, lv_group_t *group, tk_menu_t *menu)
{
    menu->group = group;

    // Automatic scroll
    lv_group_set_focus_cb(group, group_focus_cb);

    // Page
    menu_widget = lv_page_create(parent, NULL);
    lv_cont_set_fit(menu_widget, LV_FIT_PARENT);
    lv_obj_add_style(menu_widget, LV_PAGE_PART_BG, &tk_style_far_background);
    lv_obj_add_style(menu_widget, LV_PAGE_PART_SCROLLABLE, &tk_style_far_background);
    lv_obj_set_style_local_pad_inner(menu_widget, LV_PAGE_PART_SCROLLABLE, LV_STATE_DEFAULT, 0);
    lv_page_set_scrl_layout(menu_widget, LV_LAYOUT_COLUMN_MID);
    lv_obj_set_user_data(menu_widget, menu);

    for (int i = 0; i < menu->items_count; i++)
    {
        tk_menu_item_t *current_item = &(menu->items[i]);

        // Item container
        current_item->base = lv_cont_create(menu_widget, NULL);
        lv_obj_add_style(current_item->base, LV_CONT_PART_MAIN, &tk_style_menu_button);
        lv_cont_set_fit2(current_item->base, LV_FIT_PARENT, LV_FIT_TIGHT);
        lv_cont_set_layout(current_item->base, LV_LAYOUT_PRETTY_MID);
        lv_page_glue_obj(current_item->base, true);
        lv_group_add_obj(group, current_item->base);
        lv_obj_set_user_data(current_item->base, current_item);
        lv_obj_set_event_cb(current_item->base, item_base_event_cb);

        ESP_LOGW(TAG, "Pointer of user_data for %s is %p.", current_item->desc, current_item->base->user_data);

        // Item description
        current_item->label = lv_label_create(current_item->base, NULL);
        lv_label_set_text(current_item->label, current_item->desc);

        // Control
        switch (current_item->type)
        {
        case TK_MENU_ITEM_SWITCH:
            // Control
            current_item->control = lv_switch_create(current_item->base, NULL);
            lv_obj_set_user_data(current_item->control, current_item);
            lv_obj_set_event_cb(current_item->control, item_control_event_cb);

            // Initial data
            if (current_item->binding != NULL)
            {
                *(int *)current_item->binding ? lv_switch_on(current_item->control, false) : lv_switch_off(current_item->control, false);
            }
            break;

        case TK_MENU_ITEM_SLIDER:
            current_item->control = lv_slider_create(current_item->base, NULL);
            lv_obj_set_user_data(current_item->control, current_item);

            break;
        case TK_MENU_ITEM_BUTTON:
        case TK_MENU_ITEM_DDLIST:
        case TK_MENU_ITEM_LABEL:
        case TK_MENU_ITEM_SPACER:
            break;
        }
    }

    return menu_widget;
}

tk_menu_item_t *tk_menu_get_current_item(lv_group_t *group)
{
    return (tk_menu_item_t *)lv_group_get_focused(group)->user_data;
}

void tk_menu_add_item(tk_menu_t *menu, tk_menu_item_t *item)
{
}