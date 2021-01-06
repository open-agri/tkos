/**
 * @file top_bar.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Top bar generator.
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

#define TAG "Top bar"

static lv_obj_t *bar;

// Updatable widgets
lv_obj_t *clock_label;
lv_obj_t *temperature_label;
lv_obj_t *bluetooth_icon;
lv_obj_t *vehnet_icon;
lv_obj_t *location_icon;
lv_obj_t *warning_icon;
lv_obj_t *tool_icon;
lv_obj_t *title_label;

static tk_top_bar_configuration_t current_configuration;

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

    // Clock label
    if (obj == clock_label)
    {
        ESP_LOGV(TAG, "Received a refresh event for the clock label.");
        time_t time_raw;
        time(&time_raw);
        struct tm *timeinfo;
        timeinfo = localtime(&time_raw);

        char time[10] = {};
        char ampm[4] = {};
        int hours = timeinfo->tm_hour;
        if (!global_datastore.unit_settings.clock_24h)
        {
            strcpy(ampm, hours > 12 ? " PM" : " AM");
            hours = hours % 12;
        }

        char sep = ((timeinfo->tm_sec) % 2) ? ':' : ' ';

        sprintf(time, "%02d%c%02d%s", hours, sep, timeinfo->tm_min, ampm);
        lv_label_set_text(clock_label, time);
        if (current_configuration.title == NULL)
            lv_obj_align(clock_label, bar, LV_ALIGN_CENTER, 0, 0);
        else
            lv_obj_align(clock_label, bar, LV_ALIGN_IN_LEFT_MID, 8, 0);
    }
    // Temperature label
    else if (obj == temperature_label)
    {
        char temperature_text[10] = "---";
        if (global_datastore.engine_data.temp_c_available)
        {
            double temperature = global_datastore.engine_data.temp_c;
            char unit[5] = {};
            if (!global_datastore.unit_settings.celsius)
            {
                temperature = (temperature * (9.0 / 5.0)) + 32.0;
                strcpy(unit, "°F");
            }
            else
            {
                strcpy(unit, "°C");
            }

            sprintf(temperature_text, "%.1f%s", temperature, unit);
        }

        lv_label_set_text(temperature_label, temperature_text);
        lv_obj_align(temperature_label, bar, LV_ALIGN_IN_RIGHT_MID, -8, 0);
    }
    // Bluetooth icon
    else if (obj == bluetooth_icon)
    {
        lv_label_set_text(bluetooth_icon, global_datastore.bluetooth_connected ? TK_ICON_BLUETOOTH : "");
        if (current_configuration.title == NULL)
            lv_obj_align(bluetooth_icon, bar, LV_ALIGN_IN_LEFT_MID, 8, 0);
        else
            lv_obj_align(bluetooth_icon, clock_label, LV_ALIGN_OUT_RIGHT_MID, 16, 0);
    }
    // Vehnet icon
    else if (obj == vehnet_icon)
    {
        switch (global_datastore.vehnet_status)
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
    }
    // Location icon
    else if (obj == location_icon)
    {
        switch (global_datastore.gps_status)
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
    }
    // Warning icon
    else if (obj == warning_icon)
    {
        switch (global_datastore.warning_level)
        {
        case TK_WARNING_INFO:
            // Info
            lv_label_set_text(warning_icon, TK_ICON_INFO);
            lv_obj_add_style(warning_icon, LV_LABEL_PART_MAIN, &tk_style_top_bar_icon);
            break;

        case TK_WARNING_ATTENTION:
        case TK_WARNING_CRITICAL:
            // Error
            lv_label_set_text(warning_icon, TK_ICON_WARNING);
            // Change color dynamically
            lv_obj_add_style(warning_icon, LV_LABEL_PART_MAIN, (global_datastore.warning_level == TK_WARNING_ATTENTION) ? &tk_style_top_bar_icon_warn : &tk_style_top_bar_icon_error);
            break;

        case TK_WARNING_ATTENTION_FLASHING:
        case TK_WARNING_CRITICAL_FLASHING:
            /// Error flashing
            lv_label_set_text(warning_icon, TK_ICON_WARNING);
            // Change color dynamically
            lv_obj_add_style(warning_icon, LV_LABEL_PART_MAIN, (global_datastore.warning_level == TK_WARNING_ATTENTION_FLASHING) ? &tk_style_top_bar_icon_warn : &tk_style_top_bar_icon_error);

            // Flash
            struct timespec spec;
            clock_gettime(CLOCK_REALTIME, &spec);
            long ms = spec.tv_nsec / 1000000;
            lv_obj_set_style_local_text_opa(warning_icon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, ((ms % 1000) < 500) ? LV_OPA_0 : LV_OPA_100);
            break;

        case TK_WARNING_NONE:
        default:
            // Disabled
            lv_label_set_text(warning_icon, "");
            break;
        }

        lv_obj_align(warning_icon, location_icon, LV_ALIGN_OUT_RIGHT_MID, 8, 0);
    }
    // Tool icon
    else if (obj == tool_icon)
    {
        switch (global_datastore.tool_connection)
        {
        case TK_TOOL_CONNECTION_TECHNICIAN:
            lv_label_set_text(tool_icon, TK_ICON_STETHOSCOPE);
            break;
        case TK_TOOL_CONNECTION_DEVELOPER:
            lv_label_set_text(tool_icon, TK_ICON_TERMINAL);
            break;
        case TK_TOOL_CONNECTION_NONE:
        default:
            break;
        }

        lv_obj_align(tool_icon, warning_icon, LV_ALIGN_OUT_RIGHT_MID, 8, 0);
    }
    // Title label
    else if (obj == title_label)
    {
        if (current_configuration.title != NULL)
        {
            lv_label_set_text(title_label, current_configuration.title);
            lv_obj_align(title_label, bar, LV_ALIGN_CENTER, 0, 0);
        }
    }
}

/**
 * @brief The top bar generator.
 * 
 * @param configuration The configuration to use for generating the top bar.
 * @return lv_obj_t* The generated top bar.
 */
lv_obj_t *build_top_bar(tk_top_bar_configuration_t configuration)
{
    ESP_LOGD(TAG, "Building bar");

    current_configuration = configuration;

    // Background
    bar = lv_cont_create(lv_layer_top(), NULL);
    lv_obj_set_height(bar, 36);
    lv_obj_set_width(bar, 480);
    lv_obj_add_style(bar, LV_CONT_PART_MAIN, &tk_style_bar);

    // Clock
    clock_label = lv_label_create(bar, NULL);
    lv_label_set_text(clock_label, "");
    if (configuration.title == NULL)
        lv_obj_align(clock_label, bar, LV_ALIGN_CENTER, 0, 0);
    else
        lv_obj_align(clock_label, bar, LV_ALIGN_IN_LEFT_MID, 8, 0);

    // Temperature
    temperature_label = lv_label_create(bar, NULL);
    lv_label_set_text(temperature_label, "");
    lv_obj_align(temperature_label, bar, LV_ALIGN_IN_RIGHT_MID, -8, 0);

    // Icons

    // Bluetooth
    bluetooth_icon = lv_label_create(bar, NULL);
    lv_obj_add_style(bluetooth_icon, LV_LABEL_PART_MAIN, &tk_style_top_bar_icon);
    if (configuration.title == NULL)
        lv_obj_align(bluetooth_icon, bar, LV_ALIGN_IN_LEFT_MID, 8, 0);
    else
        lv_obj_align(bluetooth_icon, clock_label, LV_ALIGN_OUT_RIGHT_MID, 16, 0);

    // Vehnet
    vehnet_icon = lv_label_create(bar, NULL);
    lv_obj_add_style(vehnet_icon, LV_LABEL_PART_MAIN, &tk_style_top_bar_icon);
    lv_obj_align(vehnet_icon, bluetooth_icon, LV_ALIGN_OUT_RIGHT_MID, 8, 0);

    // Location
    location_icon = lv_label_create(bar, NULL);
    lv_obj_add_style(location_icon, LV_LABEL_PART_MAIN, &tk_style_top_bar_icon);
    lv_obj_align(location_icon, vehnet_icon, LV_ALIGN_OUT_RIGHT_MID, 8, 0);

    // Warning
    warning_icon = lv_label_create(bar, NULL);
    lv_obj_add_style(warning_icon, LV_LABEL_PART_MAIN, &tk_style_top_bar_icon);
    lv_obj_align(warning_icon, location_icon, LV_ALIGN_OUT_RIGHT_MID, 8, 0);

    // Diagnostic tool
    tool_icon = lv_label_create(bar, NULL);
    lv_obj_add_style(tool_icon, LV_LABEL_PART_MAIN, &tk_style_top_bar_icon);
    lv_obj_align(tool_icon, warning_icon, LV_ALIGN_OUT_RIGHT_MID, 8, 0);

    // Title
    title_label = lv_label_create(bar, NULL);
    lv_label_set_text(title_label, "");
    if (configuration.title != NULL)
        lv_obj_align(title_label, bar, LV_ALIGN_CENTER, 0, 0);

    // Refresh setup
    lv_obj_set_event_cb(clock_label, refresh_cb);
    lv_obj_set_event_cb(temperature_label, refresh_cb);
    lv_obj_set_event_cb(bluetooth_icon, refresh_cb);
    lv_obj_set_event_cb(vehnet_icon, refresh_cb);
    lv_obj_set_event_cb(location_icon, refresh_cb);
    lv_obj_set_event_cb(warning_icon, refresh_cb);
    lv_obj_set_event_cb(tool_icon, refresh_cb);
    lv_obj_set_event_cb(title_label, refresh_cb);

    ESP_LOGD(TAG, "Bar built successfully.");

    return bar;
}