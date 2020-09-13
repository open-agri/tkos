/**
 * @file bars.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Generator for top and bottom bars.
 * @version 0.1
 * @date 2020-09-11
 * 
 * 
 */

#ifndef BARS_H
#define BARS_H

#include <stdbool.h>
#include "lvgl/lvgl.h"

typedef void (*tk_void_callback)(void);

/**
 * @brief Represents an item of the context menu of a bar button.
 * 
 */
typedef struct
{
    char text[60];
    tk_void_callback click_callback;
} tk_menu_item_t;

/**
 * @brief Represents a bar button and its set of menu items.
 * 
 */
typedef struct
{
    char text[30];
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
    char center_symbol[16];
    tk_bottom_bar_button_t right_button;
} tk_bottom_bar_configuration_t;

/**
 * @brief The possible states of the GPS system.
 * 
 */
typedef enum {
    TK_GPS_STATUS_OFF,
    TK_GPS_STATUS_CONNECTING,
    TK_GPS_STATUS_CONNECTED
} tk_gps_status_t;

/**
 * @brief The possible states of the vehnet system.
 * 
 */
typedef enum {
    TK_VEHNET_DISABLED,
    TK_VEHNET_CONNECTING,
    TK_VEHNET_COMPLETE
} tk_vehnet_status_t;

/**
 * @brief The possible warning lights.
 * 
 */
typedef enum {
    TK_WARNING_ICON_NONE,
    TK_WARNING_ICON_INFO,
    TK_WARNING_ICON_ATTENTION,
    TK_WARNING_ICON_ATTENTION_FLASHING,
    TK_WARNING_ICON_CRITICAL,
    TK_WARNING_ICON_CRITICAL_FLASHING
} tk_warning_icon_level_t;

/**
 * @brief The possible tool connection icons.
 * 
 */
typedef enum {
    TK_TOOL_ICON_NONE,
    TK_TOOL_ICON_TECHNICIAN,
    TK_TOOL_ICON_DEVELOPER
} tk_tool_icon_t;

/**
 * @brief Represents the entire top bar.
 * 
 */
typedef struct
{
    /* LOCAL NETWORK */
    tk_vehnet_status_t local_network_status;
    unsigned int local_network_connected_device_count;

    /* GPS */
    tk_gps_status_t gps_status;

    /* WARNING */
    tk_warning_icon_level_t warning_level;

    /* BLUETOOTH CONNECTED */
    bool bluetooth_connected;

    /* CLOCK */
    bool twenty_four_hours;

    /* TOOL CONNECTED */
    tk_tool_icon_t connected_tool_icon;

    /* TEMPERATURE */
    double temp_c;
    bool celsius;

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

#endif