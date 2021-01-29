/**
 * @file notificationhandler.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief GATT notification data handler.
 * @version 0.1
 * @date 2020-12-05
 *
 *
 */

#pragma once

#include "BLE/blepeer.h"
#include "BLE/tk_uuid.h"

#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "os/os_mbuf.h"

typedef struct {
  uint16_t val_handle;
  ble_uuid_t *srv_id;
  ble_uuid_t *chr_id;
} tk_ble_notification_identifier_t;

#define NUM_INTERESTING_NOTIFICATIONS 6
extern tk_ble_notification_identifier_t
    interesting_notifications[NUM_INTERESTING_NOTIFICATIONS];

void tk_ble_handle_gatt_notification(uint16_t conn_handle, uint16_t attr_handle,
                                     struct os_mbuf *om);
