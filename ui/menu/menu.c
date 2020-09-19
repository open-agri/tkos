/**
 * @file menu.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Custom menus for encoder.
 * @version 0.1
 * @date 2020-09-17
 * 
 * 
 */

#include <math.h>

#include "menu.h"
#include "ui/views.h"

#include "esp_log.h"

#define TAG "Menu"

// The page widget
static lv_obj_t *menu_widget;
static bool lock_refresh = false;

LV_EVENT_CB_DECLARE(item_control_event_cb)
{
    // Check for user data, control and base
    if (obj->user_data == NULL)
        return;
    tk_menu_item_t *item = (tk_menu_item_t *)obj->user_data;

    if (item->control == NULL)
        return;

    if (item->base == NULL)
        return;

    // Focus the item's base
    if (e == LV_EVENT_FOCUSED)
    {
        tk_menu_t *menu = (tk_menu_t *)menu_widget->user_data;

        // Handle switches and buttons in a different mode
        if (item->type == TK_MENU_ITEM_SWITCH && lv_group_get_editing(menu->group))
        {
            lv_group_set_editing(menu->group, false);
            // Toggle the switch binding (UI will get updated later)
            lv_event_send(item->control, LV_EVENT_VALUE_CHANGED, NULL);
        }

        if (lv_group_get_editing(menu->group))
            lv_obj_add_state(item->base, LV_STATE_EDITED);
        else
        {
            lv_obj_clear_state(item->base, LV_STATE_EDITED);
            lv_obj_add_state(item->base, LV_STATE_FOCUSED);
        }
    }
    else if (e == LV_EVENT_DEFOCUSED)
    {
        lv_obj_clear_state(item->base, LV_STATE_EDITED);
        lv_obj_clear_state(item->base, LV_STATE_FOCUSED);
    }

    // A binding is needed for the following part
    if (item->binding == NULL)
        return;

    // Value changed
    if (e == LV_EVENT_VALUE_CHANGED)
    {
        lock_refresh = true;

        switch (item->type)
        {
        case TK_MENU_ITEM_SWITCH:
            // Toggle binding
            *(bool *)item->binding = !*(bool *)item->binding;
            break;
        case TK_MENU_ITEM_SLIDER:

            ;

            int raw = lv_slider_get_value(item->control);
            double perc = (double)raw / item->binding_steps;
            double out = perc * (item->binding_max - item->binding_min) + item->binding_min;

            switch (item->binding_type)
            {
            case TK_MENU_BINDING_INT:
                (*(int *)item->binding) = (int)out;
                break;
            case TK_MENU_BINDING_UINT:
                (*(unsigned int *)item->binding) = (unsigned int)out > 0 ? out : 0;
                break;
            case TK_MENU_BINDING_DOUBLE:
                (*(double *)item->binding) = out;
                break;
            }

            break;
        default:
            ESP_LOGW(TAG, "Trying to update %s binding, but its type is not implemented.", item->desc);
            break;
        }
        lock_refresh = false;

        // Refresh immediately
        lv_event_send_refresh(item->control);

        // Value change callback
        if (item->value_change_cb != NULL)
        {
            ESP_LOGI(TAG, "Calling value change callback for %s.", item->desc);
            (item->value_change_cb)(item);
        }
    }
    // Refresh
    if (e == LV_EVENT_REFRESH && !lock_refresh)
    {

        if (item->disabled)
        {
            lv_obj_add_state(item->control, LV_STATE_DISABLED);
            lv_obj_add_state(item->base, LV_STATE_DISABLED);
        }
        else
        {
            lv_obj_clear_state(item->control, LV_STATE_DISABLED);
            lv_obj_clear_state(item->base, LV_STATE_DISABLED);
        }

        switch (item->type)
        {
        case TK_MENU_ITEM_SWITCH:
            *(bool *)item->binding ? lv_switch_on(item->control, LV_ANIM_ON) : lv_switch_off(item->control, LV_ANIM_ON);
            break;
        case TK_MENU_ITEM_SLIDER:

            ;

            double perc = 0;
            switch (item->binding_type)
            {
            case TK_MENU_BINDING_INT:
                perc = (double)*(int *)item->binding;
                break;
            case TK_MENU_BINDING_UINT:
                perc = (double)*(unsigned int *)item->binding;
                break;
            case TK_MENU_BINDING_DOUBLE:
                perc = *(double *)item->binding;
                break;
            }

            perc -= item->binding_min;
            perc /= item->binding_max - item->binding_min;

            int val = (int)(perc * item->binding_steps);
            lv_slider_set_range(item->control, 0, item->binding_steps);
            lv_slider_set_value(item->control, val, LV_ANIM_OFF);           // FIXME: Does the exact opposite, animations needed otherwise slider gets stuck.

            break;
        default:
            ESP_LOGW(TAG, "Trying to refresh %s control, but its type is not implemented.", item->desc);
            break;
        }
    }
}

void group_focus_cb(lv_group_t *g)
{
    // Automatic scroll
    lv_obj_t *widget = lv_group_get_focused(g);
    lv_page_focus(menu_widget, widget, true);

    // External callback
    tk_menu_t *menu = (tk_menu_t *)menu_widget->user_data;

    if (menu->focus_change_cb != NULL)
    {
        tk_menu_item_t *widget_item = (tk_menu_item_t *)widget->user_data;
        (menu->focus_change_cb)(widget_item);
    }
}

lv_obj_t *tk_menu_create(lv_obj_t *parent, lv_group_t *group, tk_menu_t *menu)
{
    // Group save, disable wrap
    menu->group = group;
    lv_group_set_wrap(group, false);

    // Automatic scroll
    lv_group_set_focus_cb(group, group_focus_cb);

    // Page
    menu_widget = lv_page_create(parent, NULL);
    lv_cont_set_fit(menu_widget, LV_FIT_PARENT);
    lv_obj_add_style(menu_widget, LV_PAGE_PART_BG, &tk_style_far_background);
    lv_obj_add_style(menu_widget, LV_PAGE_PART_SCROLLABLE, &tk_style_far_background);
    lv_obj_set_style_local_pad_inner(menu_widget, LV_PAGE_PART_SCROLLABLE, LV_STATE_DEFAULT, 0);
    lv_page_set_scrl_layout(menu_widget, LV_LAYOUT_COLUMN_MID);

    // Page holds entire menu info
    lv_obj_set_user_data(menu_widget, menu);

    for (int i = 0; i < menu->items_count; i++)
    {
        // TODO: At the end, call value change callback if set.
        tk_menu_item_t *current_item = menu->items[i];

        // Item container
        current_item->base = lv_cont_create(menu_widget, NULL);
        lv_obj_add_style(current_item->base, LV_CONT_PART_MAIN, &tk_style_menu_button);
        lv_cont_set_fit2(current_item->base, LV_FIT_PARENT, LV_FIT_TIGHT);
        lv_cont_set_layout(current_item->base, LV_LAYOUT_PRETTY_MID);
        lv_page_glue_obj(current_item->base, true);

        // Item holds item info
        lv_obj_set_user_data(current_item->base, current_item);

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
            lv_group_add_obj(menu->group, current_item->control);

            // Initial data
            if (current_item->binding != NULL)
            {
                *(int *)current_item->binding ? lv_switch_on(current_item->control, false) : lv_switch_off(current_item->control, false);
            }
            break;

        case TK_MENU_ITEM_SLIDER:
            current_item->control = lv_slider_create(current_item->base, NULL);
            lv_obj_set_user_data(current_item->control, current_item);
            lv_obj_set_event_cb(current_item->control, item_control_event_cb);
            lv_group_add_obj(menu->group, current_item->control);

            // Hide knob when disabled
            lv_obj_add_style(current_item->control, LV_SLIDER_PART_KNOB, &tk_style_invisible_when_disabled);

            break;
        case TK_MENU_ITEM_BUTTON:
        case TK_MENU_ITEM_DDLIST:
        case TK_MENU_ITEM_LABEL:
        case TK_MENU_ITEM_SPACER:
            break;
        }

        // Disable outline
        if (current_item->control != NULL)
            lv_obj_add_style(current_item->control, LV_OBJ_PART_MAIN, &tk_style_no_outline);
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

void tk_menu_remove_item_by_desc(tk_menu_t *menu, char *desc)
{
}