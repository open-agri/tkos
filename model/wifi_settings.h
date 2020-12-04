/**
 * @file wifi_settings.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief WiFi settings.
 * @version 0.1
 * @date 2020-09-23
 *
 *
 */

#pragma once

#include <stdbool.h>

typedef struct {
  bool ap_enable;
  char ssid[32];
  char password[32];
} tk_wifi_settings_t;