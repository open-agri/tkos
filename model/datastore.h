/**
 * @file datastore.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Data store for containing all the latest data.
 * @version 0.1
 * @date 2020-09-14
 * 
 * 
 */

#pragma once

#include <stdbool.h>

#include "model/brightness.h"
#include "model/engine.h"
#include "model/location.h"
#include "model/tool.h"
#include "model/units.h"
#include "model/vehnet.h"
#include "model/warning.h"

typedef struct {
    bool bluetooth_connected;
    tk_brightness_settings_t brightness_settings;
    tk_engine_data_t engine_data;
    tk_location_data_t location_data;
    tk_gps_status_t gps_status;
    tk_tool_connection_t tool_connection;
    tk_unit_settings_t unit_settings;
    tk_vehnet_status_t vehnet_status;
    tk_warning_level_t warning_level;
} tk_datastore_t;

tk_datastore_t global_datastore;