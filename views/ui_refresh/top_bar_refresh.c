#include "top_bar_refresh.h"
#include "bars.h"

void tk_top_bar_refresher_task(lv_task_t *task)
{
    // TODO: Change this call, it's just a test for the top bar
    // TODO: No configuration struct, let's use a recursive refresh
    // TODO: Draw the bar only once on navigate
    tk_top_bar_configuration conf = {
        .bluetooth_connected = true,
        .gps_status = TK_GPS_STATUS_CONNECTING,
        .local_network_status = TK_VEHNET_CONNECTING,
        .local_network_connected_device_count = 3,
        .celsius = false,
        .twenty_four_hours = false,
        .temp_c = 35.435235,
        .warning_level = TK_WARNING_ICON_CRITICAL_FLASHING,
        .connected_tool_icon = TK_TOOL_ICON_TECHNICIAN};

    if (tk_top_bar != NULL)
        lv_obj_del(tk_top_bar);

    tk_top_bar = build_top_bar(conf);
    lv_obj_align(tk_top_bar, lv_layer_top(), LV_ALIGN_IN_TOP_MID, 0, 0);
}