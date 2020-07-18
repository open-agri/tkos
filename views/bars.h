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

typedef enum {
    TK_GPS_STATUS_OFF,
    TK_GPS_STATUS_CONNECTING,
    TK_GPS_STATUS_CONNECTED
} tk_gps_status;

typedef enum {
    TK_VEHNET_DISABLED,
    TK_VEHNET_CONNECTING,
    TK_VEHNET_COMPLETE
} tk_vehnet_status;

typedef enum {
    TK_WARNING_ICON_NONE,
    TK_WARNING_ICON_INFO,
    TK_WARNING_ICON_ATTENTION,
    TK_WARNING_ICON_ATTENTION_FLASHING,
    TK_WARNING_ICON_CRITICAL,
    TK_WARNING_ICON_CRITICAL_FLASHING
} tk_warning_icon_level;

typedef enum {
    TK_TOOL_ICON_NONE,
    TK_TOOL_ICON_TECHNICIAN,
    TK_TOOL_ICON_DEVELOPER
} tk_tool_icon;

typedef struct
{
    /* LOCAL NETWORK */
    tk_vehnet_status local_network_status;
    unsigned int local_network_connected_device_count;

    /* GPS */
    tk_gps_status gps_status;

    /* WARNING */
    tk_warning_icon_level warning_level;

    /* BLUETOOTH CONNECTED */
    bool bluetooth_connected;

    /* CLOCK */
    bool twenty_four_hours;

    /* TOOL CONNECTED */
    tk_tool_icon connected_tool_icon;

    /* TEMPERATURE */
    double temp_c;
    bool celsius;

} tk_top_bar_configuration;

lv_obj_t *build_bottom_bar(tk_bottom_bar_configuration configuration, bool original);
lv_obj_t *build_top_bar(tk_top_bar_configuration configuration);

#endif