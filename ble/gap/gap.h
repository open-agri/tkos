/**
 * @file gap.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Bluetooth LE GAP functions.
 * @version 0.1
 * @date 2020-09-20
 *
 *
 */

#pragma once

#include "services/gap/ble_svc_gap.h"
#include "host/ble_hs.h"

#define TK_BLE_GAP_CONNECTABLE true

/**
 * @brief Starts advertising with default parameters.
 * 
 * @return int 
 */
int tk_ble_gap_start_advertising(void);
