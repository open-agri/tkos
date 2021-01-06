/**
 * @file gatt.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Generic attribute server.
 * @version 0.1
 * @date 2020-09-27
 *
 *
 */

#pragma once

void tk_gatt_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);
int tk_gatt_init(void);
