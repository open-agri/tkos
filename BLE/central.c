/**
 * @file central.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief BLE Central
 * @version 0.1
 * @date 2020-12-04
 *
 *
 */

#include "esp_log.h"
#include "nvs_flash.h"
/* BLE */
#include "central.h"
#include "console/console.h"
#include "esp_nimble_hci.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "services/gap/ble_svc_gap.h"

#include "BLE/blepeer.h"
#include "BLE/notificationdelegate.h"
#include "BLE/tk_uuid.h"

#define TAG "BLE Central"

static int blecent_gap_event(struct ble_gap_event *event, void *arg);
void ble_store_config_init(void);

/**
 * Application callback.  Called when the attempt to subscribe to notifications
 * for the ANS Unread Alert Status characteristic has completed.
 */
static int blecent_on_subscribe(uint16_t conn_handle,
                                const struct ble_gatt_error *error,
                                struct ble_gatt_attr *attr, void *arg) {
  ESP_LOGI(TAG,
           "Subscribe complete; status=%d conn_handle=%d "
           "attr_handle=%d",
           error->status, conn_handle, attr->handle);

  return 0;
}

/**
 * Application callback.  Called when the write to the ANS Alert Notification
 * Control Point characteristic has completed.
 */
static int blecent_subscribe(uint16_t conn_handle, void *arg) {

  /* Subscribe to notifications for the Unread Alert Status characteristic.
   * A central enables notifications by writing two bytes (1, 0) to the
   * characteristic's client-characteristic-configuration-descriptor (CCCD).
   */

  uint8_t value[2];
  value[0] = 1;
  value[1] = 0;

  int rc;
  struct peer *peer = peer_find(conn_handle);

  for (int i = 0; i < NUM_INTERESTING_NOTIFICATIONS; i++) {
    ESP_LOGI(TAG, "Subscribing to interesting characteristic #%d.", i);

    const struct peer_dsc *dsc;

    dsc = peer_dsc_find_uuid(peer, interesting_notifications[i].srv_id,
                             interesting_notifications[i].chr_id,
                             BLE_UUID16_DECLARE(BLE_GATT_DSC_CLT_CFG_UUID16));

    if (dsc == NULL) {
      ESP_LOGW(TAG, "Error: Peer lacks a CCCD for the Unread Alert "
                    "Status characteristic. Details follow.");
      print_uuid(interesting_notifications[i].srv_id);
      print_uuid(interesting_notifications[i].chr_id);
      continue;
    }

    rc = ble_gattc_write_flat(conn_handle, dsc->dsc.handle, value, sizeof value,
                              blecent_on_subscribe, NULL);

    if (rc != 0) {
      ESP_LOGE(TAG,
               "Error: Failed to subscribe to characteristic; "
               "rc=%d.",
               rc);
      continue;
    }
  }

  // Save handles
  for (int i = 0; i < NUM_INTERESTING_NOTIFICATIONS; i++) {
    struct peer_chr *chr =
        peer_chr_find_uuid(peer, interesting_notifications[i].srv_id,
                           interesting_notifications[i].chr_id);

    if (chr != NULL) {
      interesting_notifications[i].val_handle = chr->chr.val_handle;
      ESP_LOGI(TAG,
               "Setting interesting_notifications entry #%d val_handle to %d.",
               i, chr->chr.val_handle);
    } else {
      ESP_LOGE(TAG,
               "Unable to set interesting_notifications entry #%d val_handle. "
               "chr is NULL.",
               i);
    }
  }

  return 0;
}

/**
 * Called when service discovery of the specified peer has completed.
 */
static void blecent_on_disc_complete(const struct peer *peer, int status,
                                     void *arg) {

  if (status != 0) {
    /* Service discovery failed.  Terminate the connection. */
    ESP_LOGE(TAG,
             "Error: Service discovery failed; status=%d "
             "conn_handle=%d.",
             status, peer->conn_handle);
    ble_gap_terminate(peer->conn_handle, BLE_ERR_REM_USER_CONN_TERM);
    return;
  }

  /* Service discovery has completed successfully.  Now we have a complete
   * list of services, characteristics, and descriptors that the peer
   * supports.
   */
  ESP_LOGI(TAG,
           "Service discovery complete; status=%d"
           "conn_handle=%d.",
           status, peer->conn_handle);

  /* Now perform three GATT procedures against the peer: read,
   * write, and subscribe to notifications.
   */
  // blecent_read_write_subscribe(peer);
  blecent_subscribe(peer->conn_handle, NULL);
}

/**
 * Initiates the GAP general discovery procedure.
 */
void blecent_scan(void) {
  uint8_t own_addr_type;
  struct ble_gap_disc_params disc_params;
  int rc;

  /* Figure out address to use while advertising (no privacy for now) */
  rc = ble_hs_id_infer_auto(0, &own_addr_type);
  if (rc != 0) {
    ESP_LOGE(TAG, "error determining address type; rc=%d", rc);
    return;
  }

  /* Tell the controller to filter duplicates; we don't want to process
   * repeated advertisements from the same device.
   */
  disc_params.filter_duplicates = 1;

  /**
   * Perform a passive scan.  I.e., don't send follow-up scan requests to
   * each advertiser.
   */
  disc_params.passive = 1;

  /* Use defaults for the rest of the parameters. */
  disc_params.itvl = 0;
  disc_params.window = 0;
  disc_params.filter_policy = 0;
  disc_params.limited = 0;

  rc = ble_gap_disc(own_addr_type, BLE_HS_FOREVER, &disc_params,
                    blecent_gap_event, NULL);
  if (rc != 0) {
    ESP_LOGE(TAG, "Error initiating GAP discovery procedure; rc=%d", rc);
  }
}

/**
 * Indicates whether we should try to connect to the sender of the specified
 * advertisement.  The function returns a positive result if the device
 * advertises connectability and support for the Alert Notification service.
 */
static int blecent_should_connect(const struct ble_gap_disc_desc *disc) {
  struct ble_hs_adv_fields fields;
  int rc;
  int i;

  /* The device has to be advertising connectability. */
  if (disc->event_type != BLE_HCI_ADV_RPT_EVTYPE_ADV_IND &&
      disc->event_type != BLE_HCI_ADV_RPT_EVTYPE_DIR_IND) {

    return 0;
  }

  rc = ble_hs_adv_parse_fields(&fields, disc->data, disc->length_data);
  if (rc != 0) {
    return rc;
  }

  /* The device has to advertise support for OpenAgri OTA
   */
  for (i = 0; i < fields.num_uuids128; i++) {
    if (ble_uuid_cmp(&fields.uuids128[i].u, &tk_id_common_ota.u) == 0) {
      return 1;
    }
  }

  return 0;
}

/**
 * Connects to the sender of the specified advertisement of it looks
 * interesting.  A device is "interesting" if it advertises connectability and
 * support for the Alert Notification service.
 */
static void
blecent_connect_if_interesting(const struct ble_gap_disc_desc *disc) {
  uint8_t own_addr_type;
  int rc;

  /* Don't do anything if we don't care about this advertiser. */
  if (!blecent_should_connect(disc)) {
    return;
  }

  /* Scanning must be stopped before a connection can be initiated. */
  rc = ble_gap_disc_cancel();
  if (rc != 0) {
    ESP_LOGD(TAG, "Failed to cancel scan; rc=%d", rc);
    return;
  }

  /* Figure out address to use for connect (no privacy for now) */
  rc = ble_hs_id_infer_auto(0, &own_addr_type);
  if (rc != 0) {
    ESP_LOGE(TAG, "error determining address type; rc=%d", rc);
    return;
  }

  /* Try to connect the the advertiser.  Allow 30 seconds (30000 ms) for
   * timeout.
   */

  rc = ble_gap_connect(own_addr_type, &disc->addr, 30000, NULL,
                       blecent_gap_event, NULL);
  if (rc != 0) {
    ESP_LOGE(TAG,
             "Error: Failed to connect to device; addr_type=%d "
             "addr=%s; rc=%d\n",
             disc->addr.type, addr_str(disc->addr.val), rc);
    return;
  }
}

/**
 * The nimble host executes this callback when a GAP event occurs.  The
 * application associates a GAP event callback with each connection that is
 * established.  blecent uses the same callback for all connections.
 *
 * @param event                 The event being signalled.
 * @param arg                   Application-specified argument; unused by
 *                                  blecent.
 *
 * @return                      0 if the application successfully handled the
 *                                  event; nonzero on failure.  The semantics
 *                                  of the return code is specific to the
 *                                  particular GAP event being signalled.
 */
static int blecent_gap_event(struct ble_gap_event *event, void *arg) {
  struct ble_gap_conn_desc desc;
  struct ble_hs_adv_fields fields;
  int rc;

  switch (event->type) {
  case BLE_GAP_EVENT_DISC:
    rc = ble_hs_adv_parse_fields(&fields, event->disc.data,
                                 event->disc.length_data);
    if (rc != 0) {
      return 0;
    }

    /* An advertisment report was received during GAP discovery. */
    print_adv_fields(&fields);

    /* Try to connect to the advertiser if it looks interesting. */
    blecent_connect_if_interesting(&event->disc);
    return 0;

  case BLE_GAP_EVENT_CONNECT:
    /* A new connection was established or a connection attempt failed. */
    if (event->connect.status == 0) {
      /* Connection successfully established. */
      ESP_LOGI(TAG, "Connection established");

      rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
      assert(rc == 0);
      print_conn_desc(&desc);

      /* Remember peer. */
      rc = peer_add(event->connect.conn_handle);
      if (rc != 0) {
        ESP_LOGE(TAG, "Failed to add peer; rc=%d", rc);
        return 0;
      }

      /* Perform service discovery. */
      rc = peer_disc_all(event->connect.conn_handle, blecent_on_disc_complete,
                         NULL);
      if (rc != 0) {
        ESP_LOGE(TAG, "Failed to discover services; rc=%d", rc);
        return 0;
      }
    } else {
      /* Connection attempt failed; resume scanning. */
      ESP_LOGE(TAG, "Error: Connection failed; status=%d",
               event->connect.status);
      blecent_scan();
    }

    return 0;

  case BLE_GAP_EVENT_DISCONNECT:
    /* Connection terminated. */
    ESP_LOGI(TAG, "disconnect; reason=%d", event->disconnect.reason);
    print_conn_desc(&event->disconnect.conn);

    /* Forget about peer. */
    peer_delete(event->disconnect.conn.conn_handle);

    /* Resume scanning. */
    blecent_scan();
    return 0;

  case BLE_GAP_EVENT_DISC_COMPLETE:
    ESP_LOGI(TAG, "discovery complete; reason=%d", event->disc_complete.reason);
    return 0;

  case BLE_GAP_EVENT_ENC_CHANGE:
    /* Encryption has been enabled or disabled for this connection. */
    ESP_LOGI(TAG, "encryption change event; status=%d ",
             event->enc_change.status);
    rc = ble_gap_conn_find(event->enc_change.conn_handle, &desc);
    assert(rc == 0);
    print_conn_desc(&desc);
    return 0;

  case BLE_GAP_EVENT_NOTIFY_RX:
    /* Peer sent us a notification or indication. */
    ESP_LOGD(TAG,
             "received %s; conn_handle=%d attr_handle=%d "
             "attr_len=%d",
             event->notify_rx.indication ? "indication" : "notification",
             event->notify_rx.conn_handle, event->notify_rx.attr_handle,
             OS_MBUF_PKTLEN(event->notify_rx.om));

    // Send notification data to tkos
    if (!(event->notify_rx.indication)) {
      // Is a notification
      tk_ble_handle_gatt_notification(event->notify_rx.conn_handle,
                                      event->notify_rx.attr_handle,
                                      event->notify_rx.om);
    }

    /* Attribute data is contained in event->notify_rx.attr_data. */
    return 0;

  case BLE_GAP_EVENT_MTU:
    ESP_LOGI(TAG, "mtu update event; conn_handle=%d cid=%d mtu=%d",
             event->mtu.conn_handle, event->mtu.channel_id, event->mtu.value);
    return 0;

  case BLE_GAP_EVENT_REPEAT_PAIRING:
    /* We already have a bond with the peer, but it is attempting to
     * establish a new secure link.  This app sacrifices security for
     * convenience: just throw away the old bond and accept the new link.
     */

    /* Delete the old bond. */
    rc = ble_gap_conn_find(event->repeat_pairing.conn_handle, &desc);
    assert(rc == 0);
    ble_store_util_delete_peer(&desc.peer_id_addr);

    /* Return BLE_GAP_REPEAT_PAIRING_RETRY to indicate that the host should
     * continue with the pairing operation.
     */
    return BLE_GAP_REPEAT_PAIRING_RETRY;

  default:
    return 0;
  }
}

// static void
// blecent_on_reset(int reason)
// {
//     ESP_LOGE(TAG, "Resetting state; reason=%d", reason);
// }

// static void
// blecent_on_sync(void)
// {
//     int rc;

//     /* Make sure we have proper identity address set (public preferred) */
//     rc = ble_hs_util_ensure_addr(0);
//     assert(rc == 0);

//     /* Begin scanning for a peripheral to connect to. */
//     blecent_scan();
// }

// void blecent_host_task(void *param)
// {
//     ESP_LOGI(TAG, "BLE Host Task Started");
//     /* This function will return only when nimble_port_stop() is executed */
//     nimble_port_run();

//     nimble_port_freertos_deinit();
// }

// void
// app_main(void)
// {
//     int rc;
//     /* Initialize NVS — it is used to store PHY calibration data */
//     esp_err_t ret = nvs_flash_init();
//     if  (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret ==
//     ESP_ERR_NVS_NEW_VERSION_FOUND) {
//         ESP_ERROR_CHECK(nvs_flash_erase());
//         ret = nvs_flash_init();
//     }
//     ESP_ERROR_CHECK(ret);

//     ESP_ERROR_CHECK(esp_nimble_hci_and_controller_init());

//     nimble_port_init();
//     /* Configure the host. */
//     ble_hs_cfg.reset_cb = blecent_on_reset;
//     ble_hs_cfg.sync_cb = blecent_on_sync;
//     ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

//     /* Initialize data structures to track connected peers. */
//     rc = peer_init(MYNEWT_VAL(BLE_MAX_CONNECTIONS), 64, 64, 64);
//     assert(rc == 0);

//     /* Set the default device name. */
//     rc = ble_svc_gap_device_name_set("nimble-blecent");
//     assert(rc == 0);

//     /* XXX Need to have template for store */
//     ble_store_config_init();

//     nimble_port_freertos_init(blecent_host_task);

// }