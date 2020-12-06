/**
 * @file ble.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief BLE peripheral.
 * @version 0.1
 * @date 2020-09-26
 * 
 */

#pragma once

#if CONFIG_TK_ENGINESENSE
#define BT_NAME_PREFIX  "TKES"
#elif CONFIG_TK_NAVIS
#define BT_NAME_PREFIX "TKN"
#else
#define BT_NAME_PREFIX "TK"
#endif

void tk_ble_init(void);