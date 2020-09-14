/**
 * @file bottom_bar.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Bottom bar generator.
 * @version 0.1
 * @date 2020-09-14
 * 
 * 
 */

#include "lvgl/lvgl.h"

#include "ui/bars/bars.h"
#include "ui/styles/tk_style.h"
#include "ui/fonts/icons.h"
#include "ui/views.h"

#include "model/datastore.h"

#include "esp_log.h"

#include <stdio.h>
#include <time.h>

#define TAG "Bottom bar"

lv_obj_t *bar;

// Updatable widgets
lv_obj_t *left_button_label;
lv_obj_t *center_label;
lv_obj_t *right_button_label;

tk_bottom_bar_configuration_t current_configuration;
tk_bottom_bar_configuration_t original_configuration;

bool menu_open;
bool menu_flag;

lv_obj_t *menu;
lv_group_t *menu_group;

/**
 * @brief Pushes new data to the widgets when they receive a refresh event.
 * 
 * @param obj The widget that called this callback function.
 * @param event The event that the widget received.
 */
static void refresh_cb(lv_obj_t *obj, lv_event_t event)
{
    if (event != LV_EVENT_REFRESH)
        return;

    // Left button label
    if (obj == left_button_label)
    {
        // Icon
        char label_text[40] = {};
        if (current_configuration.left_button.items_count > 0 && !menu_open)
            strncpy(label_text, LV_SYMBOL_UP "   ", 7);

        strncat(label_text, current_configuration.left_button.text, 34);

        // Label update
        lv_label_set_text(left_button_label, label_text);
    }
    // Center label
    else if (obj == center_label)
    {
        lv_label_set_text(center_label, current_configuration.center_text);
        lv_obj_align(center_label, bar, LV_ALIGN_CENTER, 0, 0);
    }
    // Right button label
    else if (obj == right_button_label)
    {
        // Icon
        char label_text[40] = {};
        if (current_configuration.right_button.items_count > 0 && !menu_open)
            strncpy(label_text, LV_SYMBOL_UP "   ", 7);

        strncat(label_text, current_configuration.right_button.text, 34);

        // Label update
        lv_label_set_text(right_button_label, label_text);
    }
}

/**
 * @brief Destroys the entire CPU. No, it hides a menu and restores the normal bottom bar configuration.
 * 
 * @param menu The menu to hide.
 */
void hide_menu(lv_obj_t *menu)
{

    ESP_LOGI(TAG, "Hiding menu.");

    // Hide the menu
    lv_obj_del(menu);

    // Reset variables and configuration
    menu_open = false;

    // Rebuild the bar with original bb_conf
    lv_obj_del(bar);
    build_bottom_bar(original_configuration, false);

    // Note: build_bottom_bar automatically sets bar pointer
    lv_obj_align(bar, lv_scr_act(), LV_ALIGN_IN_BOTTOM_MID, 0, 0);
}

/**
 * @brief Shows a menu and adjusts the two bottom bar buttons by creating a temporary bottom bar configuration.
 * 
 * @param current_bb_conf The bottom bar configuration that from which to get the menu items.
 * @param left Whether the menu should be shown on the left or right side of the screen.
 */
void show_menu(tk_bottom_bar_configuration_t current_bb_conf, bool left)
{

    unsigned int items = left ? current_bb_conf.left_button.items_count : current_bb_conf.right_button.items_count;
    tk_menu_item_t *menu_items = left ? current_bb_conf.left_button.menu : current_bb_conf.right_button.menu;

    ESP_LOGI(TAG, "Building %s menu with %d items.", left ? "left" : "right", items);

    // Menu generation
    // TODO: Automatic resize
    menu = lv_list_create(lv_scr_act(), NULL);
    menu_group = lv_group_create();
    lv_group_add_obj(menu_group, menu);
    lv_list_set_scrollbar_mode(menu, LV_SCROLLBAR_MODE_AUTO);
    lv_list_set_anim_time(menu, 200);
    lv_obj_set_width(menu, 140);
    lv_obj_add_style(menu, LV_LIST_PART_BG, &tk_style_menu);
    lv_obj_add_style(menu, LV_LIST_PART_SCROLLABLE, &tk_style_menu);

    lv_obj_t *btn;
    unsigned int tot_height = 0;
    for (int i = 0; i < items; i++)
    {
        btn = lv_list_add_btn(menu, NULL, menu_items[i].text);
        lv_obj_add_style(btn, LV_BTN_PART_MAIN, &tk_style_menu_button);
        lv_obj_set_style_local_pad_ver(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 12);
        tot_height += lv_obj_get_height(btn);

        // Set callback to be retrieved later in user_data
        // TODO: Create a callback which executes this pointer on tap, for eventual future touchscreen systems
        lv_obj_set_user_data(btn, menu_items[i].click_callback);
    }

    lv_indev_set_group(encoder_indev, menu_group);
    lv_group_focus_obj(menu);
    lv_group_set_editing(menu_group, true);

    // Update height
    lv_obj_set_height(menu, (tot_height <= 276) ? tot_height : 276);
    lv_obj_align(menu, bar, left ? LV_ALIGN_OUT_TOP_LEFT : LV_ALIGN_OUT_TOP_RIGHT, left ? 8 : -8, -8);

    // Update variables
    menu_open = true;

    // Update bar
    lv_obj_del(bar);
    tk_bottom_bar_configuration_t temp_bb_conf = current_bb_conf;
    strcpy(temp_bb_conf.right_button.text, "Select   " LV_SYMBOL_OK);
    strcpy(temp_bb_conf.left_button.text, LV_SYMBOL_LEFT "   Back");
    bar = build_bottom_bar(temp_bb_conf, false);
    lv_obj_align(bar, lv_scr_act(), LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    lv_obj_move_foreground(bar);
}

/**
 * @brief Guess what? The bar's left button event callback.
 * 
 * @param obj The sender.
 * @param event The received event.
 */
static void left_button_event_callback(lv_obj_t *obj, lv_event_t event)
{

    switch (event)
    {
    case LV_EVENT_SHORT_CLICKED:

        ESP_LOGD(TAG, "Left button short clicked.");

        // Clicked: execute callback when menus closed, close menu when open
        if (menu_open)
        {
            // Close menu
            hide_menu(menu);
        }
        else
        {
            if (original_configuration.left_button.click_callback != NULL)
            {
                ESP_LOGD(TAG, "Calling callback function.");
                (original_configuration.left_button.click_callback)();
            }
            else
            {
                ESP_LOGW(TAG, "No callback function available. This could be an unintended behaviour.");
            }
        }

        break;

    case LV_EVENT_LONG_PRESSED:

        ESP_LOGD(TAG, "Left button long pressed.");

        // Long press: show menu when both are closed and a menu is available
        if (!menu_open && original_configuration.left_button.items_count > 0)
        {
            ESP_LOGD(TAG, "Flagging menu for opening.");

            // Using a flag in order to delay the appearance of the menu on button release
            menu_flag = true;
        }
        break;

    case LV_EVENT_RELEASED:

        ESP_LOGD(TAG, "Left button released.");

        // No continued pressure
        lv_obj_set_state(obj, LV_STATE_DEFAULT);

        if (menu_flag)
        {
            ESP_LOGD(TAG, "Triggering menu opening.");
            show_menu(original_configuration, true);
            menu_flag = false;
        }
        break;
    }
}

/**
 * @brief The bar's right button event callback.
 * 
 * @param obj The sender.
 * @param event The received event.
 */
static void right_button_event_callback(lv_obj_t *obj, lv_event_t event)
{

    switch (event)
    {
    case LV_EVENT_SHORT_CLICKED:

        ESP_LOGD(TAG, "Right button short clicked.");

        // Clicked: execute callback when menus closed, select item when open
        if (menu_open)
        {

            ESP_LOGD(TAG, "Menu is open, calling button specific callback.");

            // Select item (execute function pointed by user data of the focused button)
            tk_void_callback cb = (tk_void_callback)lv_list_get_btn_selected(menu)->user_data;
            if (cb != NULL)
            {
                (cb)();
            }
            else
            {
                ESP_LOGW(TAG, "The selected button does not have a click callback. Please create one or remove the button.");
            }

            // Close menu
            hide_menu(menu);
        }
        else
        {
            if (original_configuration.right_button.click_callback != NULL)
            {
                ESP_LOGD(TAG, "Calling callback function.");
                (original_configuration.right_button.click_callback)();
            }
            else
            {
                ESP_LOGW(TAG, "No callback function available. This could be an unintended behaviour.");
            }
        }

        break;

    case LV_EVENT_LONG_PRESSED:

        ESP_LOGD(TAG, "Right button long pressed.");

        // Long press: show menu when both are closed and a menu is available
        if (!menu_open && original_configuration.right_button.items_count > 0)
        {

            ESP_LOGD(TAG, "Flagging menu for opening.");

            // Using a flag in order to delay the appearance of the menu on button release
            menu_flag = true;
        }
        break;

    case LV_EVENT_RELEASED:

        ESP_LOGD(TAG, "Right button released.");

        // No continued pressure
        lv_obj_set_state(obj, LV_STATE_DEFAULT);

        if (menu_flag)
        {
            ESP_LOGD(TAG, "Triggering menu opening.");
            show_menu(original_configuration, false);
            menu_flag = false;
        }
        break;
    }
}

/**
 * @brief The bottom bar generator.
 * 
 * @param configuration The configuration to use for generating the bottom bar.
 * @param original Set to true to save this configuration to `original_bottom_bar_configuration`.
 * @return lv_obj_t* The generated bottom bar.
 */
lv_obj_t *build_bottom_bar(tk_bottom_bar_configuration_t configuration, bool original)
{

    ESP_LOGD(TAG, "Building bottom bar.");

    current_configuration = configuration;

    // Background
    bar = lv_cont_create(lv_layer_top(), NULL);
    lv_obj_set_height(bar, 36);
    lv_obj_set_width(bar, 480);
    lv_obj_add_style(bar, LV_CONT_PART_MAIN, &tk_style_bar);

    // Left button
    if (configuration.left_button.text != NULL)
    {
        lv_obj_t *left_button = lv_btn_create(bar, NULL);
        lv_obj_set_size(left_button, 120, 36);
        lv_obj_set_event_cb(left_button, left_button_event_callback);
        lv_obj_add_style(left_button, LV_BTN_PART_MAIN, &tk_style_menu_button);

        // Label
        left_button_label = lv_label_create(left_button, NULL);
    }

    // Center label
    center_label = lv_label_create(bar, NULL);
    lv_obj_align(center_label, bar, LV_ALIGN_CENTER, 0, 0);

    // Right button
    if (configuration.right_button.text != NULL)
    {
        lv_obj_t *right_button = lv_btn_create(bar, NULL);
        lv_obj_set_size(right_button, 120, 36);
        lv_obj_set_event_cb(right_button, right_button_event_callback);
        lv_obj_add_style(right_button, LV_BTN_PART_MAIN, &tk_style_menu_button);

        // Label
        right_button_label = lv_label_create(right_button, NULL);
    }

    // Save configuration
    if (original)
        original_configuration = configuration;

    // Refresh setup
    lv_obj_set_event_cb(left_button_label, refresh_cb);
    lv_obj_set_event_cb(center_label, refresh_cb);
    lv_obj_set_event_cb(right_button_label, refresh_cb);

    ESP_LOGD(TAG, "Bottom bar built successfully%s.", original ? " and saved" : "");

    return bar;
}