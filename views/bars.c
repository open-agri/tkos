/**
 * @file bars.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Generator for top and bottom bars. TODO: NOOOOOOOOOOOO, EACH SHOULD HAVE A FILE
 * @version 0.1
 * @date 2020-09-11
 * 
 * 
 */

#include "lvgl/lvgl.h"
#include "styles/tk_style.h"
#include "fonts/icons.h"
#include "../tkos.h"
#include "bars.h"
#include "esp_log.h"

#include <stdio.h>
#include <time.h> // For flashing icons and actual time

#define TAG "bars"

tk_bottom_bar_configuration_t original_bottom_bar_configuration;

bool menu_open = false;
bool menu_flag = false;

lv_obj_t *menu;
lv_group_t *menu_group;
lv_obj_t *bottom_bar;

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
    lv_obj_del(bottom_bar);
    build_bottom_bar(original_bottom_bar_configuration, false);

    // Note: build_bottom_bar automatically sets bottom_bar pointer
    lv_obj_align(bottom_bar, lv_scr_act(), LV_ALIGN_IN_BOTTOM_MID, 0, 0);
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
    lv_obj_align(menu, bottom_bar, left ? LV_ALIGN_OUT_TOP_LEFT : LV_ALIGN_OUT_TOP_RIGHT, left ? 8 : -8, -8);

    // Update variables
    menu_open = true;

    // Update bar
    lv_obj_del(bottom_bar);
    tk_bottom_bar_configuration_t temp_bb_conf = current_bb_conf;
    strcpy(temp_bb_conf.right_button.text, "Select   " LV_SYMBOL_OK);
    strcpy(temp_bb_conf.left_button.text, LV_SYMBOL_LEFT "   Back");
    bottom_bar = build_bottom_bar(temp_bb_conf, false);
    lv_obj_align(bottom_bar, lv_scr_act(), LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    lv_obj_move_foreground(bottom_bar);
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
            if (original_bottom_bar_configuration.left_button.click_callback != NULL)
            {
                ESP_LOGD(TAG, "Calling callback function.");
                (original_bottom_bar_configuration.left_button.click_callback)();
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
        if (!menu_open && original_bottom_bar_configuration.left_button.items_count > 0)
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
            show_menu(original_bottom_bar_configuration, true);
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
            ((tk_void_callback)(lv_list_get_btn_selected(menu)->user_data))();
            // Close menu
            hide_menu(menu);
        }
        else
        {
            if (original_bottom_bar_configuration.right_button.click_callback != NULL)
            {
                ESP_LOGD(TAG, "Calling callback function.");
                (original_bottom_bar_configuration.right_button.click_callback)();
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
        if (!menu_open && original_bottom_bar_configuration.right_button.items_count > 0)
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
            show_menu(original_bottom_bar_configuration, false);
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

    /* BACKGROUND */

    /* SAVE IN "GLOBAL" POINTER FOR ALLOWING INTERNAL CHANGES */
    bottom_bar = lv_cont_create(lv_scr_act(), NULL);
    lv_obj_set_height(bottom_bar, 36);
    lv_obj_set_width(bottom_bar, 480);
    lv_obj_add_style(bottom_bar, LV_CONT_PART_MAIN, &tk_style_bar);

    /* LEFT BUTTON */
    if (strlen(configuration.left_button.text))
    {
        lv_obj_t *left_button = lv_btn_create(bottom_bar, NULL);
        lv_obj_set_size(left_button, 120, 36);
        lv_obj_set_event_cb(left_button, left_button_event_callback);
        lv_obj_add_style(left_button, LV_BTN_PART_MAIN, &tk_style_menu_button);

        /* ICON PREPARATION */
        char label_text[40] = {};
        if (configuration.left_button.items_count > 0 && !menu_open)
            strcpy(label_text, LV_SYMBOL_UP "   ");

        strcat(label_text, configuration.left_button.text);

        /* LABEL */
        lv_obj_t *label = lv_label_create(left_button, NULL);
        lv_label_set_text(label, label_text);
    }

    /* CENTER LABEL */
    lv_obj_t *center_label = lv_label_create(bottom_bar, NULL);
    lv_label_set_text(center_label, configuration.center_symbol);
    lv_obj_align(center_label, bottom_bar, LV_ALIGN_CENTER, 0, 0);

    /* RIGHT BUTTON */
    if (strlen(configuration.right_button.text))
    {
        lv_obj_t *right_button = lv_btn_create(bottom_bar, NULL);
        lv_obj_set_size(right_button, 120, 36);
        lv_obj_set_event_cb(right_button, right_button_event_callback);
        lv_obj_add_style(right_button, LV_BTN_PART_MAIN, &tk_style_menu_button);

        lv_obj_align(right_button, bottom_bar, LV_ALIGN_IN_RIGHT_MID, 0, 0);

        /* ICON PREPARATION */
        char label_text[40];
        strcpy(label_text, configuration.right_button.text);
        if (configuration.right_button.items_count > 0 && !menu_open)
            strcat(label_text, "   " LV_SYMBOL_UP);

        /* LABEL */
        lv_obj_t *label = lv_label_create(right_button, NULL);
        lv_label_set_text(label, label_text);
    }

    /* SAVE CONF AND POINTER */
    if (original)
        original_bottom_bar_configuration = configuration;

    ESP_LOGD(TAG, "Bottom bar built successfully%s.", original ? " and saved" : "");

    return bottom_bar;
}

/**
 * @brief The top bar generator.
 * 
 * @param configuration The configuration to use for generating the top bar.
 * @return lv_obj_t* The generated top bar.
 */
lv_obj_t *build_top_bar(tk_top_bar_configuration_t configuration)
{

    ESP_LOGD(TAG, "Building top bar.");

    /* BACKGROUND */
    lv_obj_t *tk_top_bar = lv_cont_create(lv_layer_top(), NULL);
    lv_obj_set_height(tk_top_bar, 36);
    lv_obj_set_width(tk_top_bar, 480);
    lv_obj_add_style(tk_top_bar, LV_CONT_PART_MAIN, &tk_style_bar);

    /* CLOCK */
    time_t time_raw;
    time(&time_raw);
    struct tm *timeinfo;
    timeinfo = localtime(&time_raw);

    char time[10] = {};
    char ampm[4] = {};
    int hours = timeinfo->tm_hour;
    if (!configuration.twenty_four_hours)
    {
        strncpy(ampm, hours > 12 ? " PM" : " AM", 4);
        hours = hours % 12;
    }

    char sep = ((timeinfo->tm_sec) % 2) ? ':' : ' ';

    sprintf(time, "%02d%c%02d%s", hours, sep, timeinfo->tm_min, ampm);
    lv_obj_t *clock_label = lv_label_create(tk_top_bar, NULL);
    lv_label_set_text(clock_label, time);
    lv_obj_align(clock_label, tk_top_bar, LV_ALIGN_CENTER, 0, 0);

    /* TEMPERATURE */
    double temperature = configuration.temp_c;
    char unit[5] = {};
    if (!configuration.celsius)
    {
        temperature = (temperature * (9.0 / 5.0)) + 32.0;
        strncpy(unit, "°F", 4);
    }
    else
    {
        strncpy(unit, "°C", 4);
    }

    char temperature_text[10] = {};
    sprintf(temperature_text, "%.1f%s", temperature, unit);

    lv_obj_t *temperature_label = lv_label_create(tk_top_bar, NULL);
    lv_label_set_text(temperature_label, temperature_text);
    lv_obj_align(temperature_label, tk_top_bar, LV_ALIGN_IN_RIGHT_MID, -8, 0);

    /* BLUETOOTH */
    lv_obj_t *bluetooth_icon = lv_label_create(tk_top_bar, NULL);
    lv_obj_add_style(bluetooth_icon, LV_LABEL_PART_MAIN, &tk_style_top_bar_icon);
    lv_label_set_text(bluetooth_icon, (configuration.bluetooth_connected) ? TK_ICON_BLUETOOTH : "");
    lv_obj_align(bluetooth_icon, tk_top_bar, LV_ALIGN_IN_LEFT_MID, 8, 0);

    /* LOCAL NETWORK */
    lv_obj_t *vehnet_icon = lv_label_create(tk_top_bar, NULL);
    lv_obj_add_style(vehnet_icon, LV_LABEL_PART_MAIN, &tk_style_top_bar_icon);

    switch (configuration.local_network_status)
    {
    case TK_VEHNET_COMPLETE:
        // Complete
        lv_label_set_text(vehnet_icon, TK_ICON_LAN);
        break;

    case TK_VEHNET_CONNECTING:
        // Connecting
        lv_label_set_text(vehnet_icon, TK_ICON_LAN);

        // Flash
        struct timespec spec;
        clock_gettime(CLOCK_REALTIME, &spec);
        long ms = spec.tv_nsec / 1000000;
        lv_obj_set_style_local_text_opa(vehnet_icon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, ((ms % 1000) < 500) ? LV_OPA_0 : LV_OPA_100);
        break;
    case TK_VEHNET_DISABLED:
    default:
        // Disabled
        lv_label_set_text(vehnet_icon, "");
        break;
    }

    lv_obj_align(vehnet_icon, bluetooth_icon, LV_ALIGN_OUT_RIGHT_MID, 8, 0);

    /* LOCATION */
    lv_obj_t *location_icon = lv_label_create(tk_top_bar, NULL);
    lv_obj_add_style(location_icon, LV_LABEL_PART_MAIN, &tk_style_top_bar_icon);

    switch (configuration.gps_status)
    {
    case TK_GPS_STATUS_CONNECTED:
        // Connected
        lv_label_set_text(location_icon, TK_ICON_LOCATION);
        break;

    case TK_GPS_STATUS_CONNECTING:
        // Connecting
        lv_label_set_text(location_icon, TK_ICON_LOCATION);

        // Flash
        struct timespec spec;
        clock_gettime(CLOCK_REALTIME, &spec);
        long ms = spec.tv_nsec / 1000000;
        lv_obj_set_style_local_text_opa(location_icon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, ((ms % 1000) >= 500) ? LV_OPA_0 : LV_OPA_100);
        break;

    case TK_GPS_STATUS_OFF:
    default:
        // Disabled
        lv_label_set_text(location_icon, "");
        break;
    }

    lv_obj_align(location_icon, vehnet_icon, LV_ALIGN_OUT_RIGHT_MID, 8, 0);

    /* WARNING */
    lv_obj_t *warning_icon = lv_label_create(tk_top_bar, NULL);

    switch (configuration.warning_level)
    {
    case TK_WARNING_ICON_INFO:
        // Info
        lv_label_set_text(warning_icon, TK_ICON_INFO);
        lv_obj_add_style(warning_icon, LV_LABEL_PART_MAIN, &tk_style_top_bar_icon);
        break;

    case TK_WARNING_ICON_ATTENTION:
    case TK_WARNING_ICON_CRITICAL:
        // Error
        lv_label_set_text(warning_icon, TK_ICON_WARNING);
        // Change color dynamically
        lv_obj_add_style(warning_icon, LV_LABEL_PART_MAIN, (configuration.warning_level == TK_WARNING_ICON_ATTENTION) ? &tk_style_top_bar_icon_warn : &tk_style_top_bar_icon_error);
        break;

    case TK_WARNING_ICON_ATTENTION_FLASHING:
    case TK_WARNING_ICON_CRITICAL_FLASHING:
        /// Error flashing
        lv_label_set_text(warning_icon, TK_ICON_WARNING);
        // Change color dynamically
        lv_obj_add_style(warning_icon, LV_LABEL_PART_MAIN, (configuration.warning_level == TK_WARNING_ICON_ATTENTION_FLASHING) ? &tk_style_top_bar_icon_warn : &tk_style_top_bar_icon_error);

        // Flash
        struct timespec spec;
        clock_gettime(CLOCK_REALTIME, &spec);
        long ms = spec.tv_nsec / 1000000;
        lv_obj_set_style_local_text_opa(warning_icon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, ((ms % 1000) < 500) ? LV_OPA_0 : LV_OPA_100);
        break;

    case TK_WARNING_ICON_NONE:
    default:
        // Disabled
        lv_label_set_text(warning_icon, "");
        break;
    }

    lv_obj_align(warning_icon, location_icon, LV_ALIGN_OUT_RIGHT_MID, 8, 0);

    /* DIAGNOSIS */
    lv_obj_t *diag_icon = lv_label_create(tk_top_bar, NULL);
    lv_obj_add_style(diag_icon, LV_LABEL_PART_MAIN, &tk_style_top_bar_icon);
    switch (configuration.connected_tool_icon)
    {
    case TK_TOOL_ICON_TECHNICIAN:
        lv_label_set_text(diag_icon, TK_ICON_STETHOSCOPE);
        break;
    case TK_TOOL_ICON_DEVELOPER:
        lv_label_set_text(diag_icon, TK_ICON_TERMINAL);
        break;
    case TK_TOOL_ICON_NONE:
    default:
        break;
    }

    lv_obj_align(diag_icon, warning_icon, LV_ALIGN_OUT_RIGHT_MID, 8, 0);

    ESP_LOGD(TAG, "Top bar built successfully.");

    return tk_top_bar;
}