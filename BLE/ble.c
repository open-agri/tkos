/**
 * @file ble.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief BLE peripheral.
 * @version 0.1
 * @date 2020-09-26
 *
 *
 */

#include "esp_bt.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"

/* BLE */
#include "esp_nimble_hci.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "services/gap/ble_svc_gap.h"

#include "ble.h"
#include "blepeer.h"
#include "central.h"
#include "gatt.h"
#include "tk_uuid.h"

#define TAG "BLE"

static uint8_t own_addr_type;

void ble_store_config_init(void);
static void ble_advertise(void);

#define SERVICE_COUNT_128 1
const ble_uuid128_t service_ids_128[SERVICE_COUNT_128] = {tk_id_common_ota};

#define SERVICE_COUNT_16 1
const ble_uuid16_t service_ids_16[SERVICE_COUNT_16] = {tk_id_device_info};

/**
 * The nimble host executes this callback when a GAP event occurs.  The
 * application associates a GAP event callback with each connection that forms.
 * bleprph uses the same callback for all connections.
 *
 * @param event                 The type of event being signalled.
 * @param ctxt                  Various information pertaining to the event.
 * @param arg                   Application-specified argument; unused by
 *                                  bleprph.
 *
 * @return                      0 if the application successfully handled the
 *                                  event; nonzero on failure.  The semantics
 *                                  of the return code is specific to the
 *                                  particular GAP event being signalled.
 */
static int ble_gap_event_cb(struct ble_gap_event *event, void *arg) {
  struct ble_gap_conn_desc desc;
  int rc;

  switch (event->type) {
  case BLE_GAP_EVENT_CONNECT:
    /* A new connection was established or a connection attempt failed. */
    ESP_LOGI(TAG, "Connection %s; status=%x",
             event->connect.status == 0 ? "established" : "failed",
             event->connect.status);
    if (event->connect.status == 0) {
      rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
      assert(rc == 0);
    }

    if (event->connect.status != 0) {
      /* Connection failed; resume advertising. */
      ble_advertise();
    }
    return 0;

  case BLE_GAP_EVENT_DISCONNECT:
    ESP_LOGI(TAG, "Disconnected; reason=%x", event->disconnect.reason);

    /* Connection terminated; resume advertising. */
    ble_advertise();
    return 0;

  case BLE_GAP_EVENT_CONN_UPDATE:
    /* The central has updated the connection parameters. */
    ESP_LOGI(TAG, "Connection updated; status=%x", event->conn_update.status);
    rc = ble_gap_conn_find(event->conn_update.conn_handle, &desc);
    assert(rc == 0);
    return 0;

  case BLE_GAP_EVENT_ADV_COMPLETE:
    ESP_LOGI(TAG, "Advertise complete; reason=%x", event->adv_complete.reason);
    ble_advertise();
    return 0;

  case BLE_GAP_EVENT_ENC_CHANGE:
    /* Encryption has been enabled or disabled for this connection. */
    ESP_LOGI(TAG, "Encryption change event; status=%x",
             event->enc_change.status);
    rc = ble_gap_conn_find(event->enc_change.conn_handle, &desc);
    assert(rc == 0);
    return 0;

  case BLE_GAP_EVENT_SUBSCRIBE:
    ESP_LOGI(TAG,
             "Subscribe event; conn_handle=%d attr_handle=%d "
             "reason=%x prevn=%d curn=%d previ=%d curi=%d",
             event->subscribe.conn_handle, event->subscribe.attr_handle,
             event->subscribe.reason, event->subscribe.prev_notify,
             event->subscribe.cur_notify, event->subscribe.prev_indicate,
             event->subscribe.cur_indicate);
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
    break;
  }

  return 0;
}

static void ble_advertise(void) {
  struct ble_gap_adv_params adv_params;
  struct ble_hs_adv_fields fields;
  const char *name;
  int rc;

  memset(&fields, 0, sizeof fields);

  // Discoverable and BLE-only
  fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

  // Auto TX power
  fields.tx_pwr_lvl_is_present = 1;
  fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;

  // Get name from host and advertise it
  name = ble_svc_gap_device_name();
  fields.name = (uint8_t *)name;
  fields.name_len = strlen(name);
  fields.name_is_complete = 1;

  // Set temp fields
  rc = ble_gap_adv_set_fields(&fields);
  if (rc) {
    ESP_LOGE(TAG, "Error while setting preliminary GAP fields (error %d).", rc);
    return;
  }

  memset(&fields, 0, sizeof fields);

  // Initialize 128-bit UUIDs
  fields.num_uuids128 = 1;
  fields.uuids128 = service_ids_128;
  fields.uuids128_is_complete = (SERVICE_COUNT_128 <= 1);

  rc = ble_gap_adv_set_fields(&fields);
  if (rc) {
    ESP_LOGE(TAG,
             "Error while setting first service 128-bit UUID in GAP fields "
             "(error %d).",
             rc);
    return;
  }

  if (SERVICE_COUNT_128 > 1) {
    struct ble_hs_adv_fields scan_response_fields;
    memset(&scan_response_fields, 0, sizeof scan_response_fields);
    scan_response_fields.uuids128 = &(service_ids_128[1]);
    scan_response_fields.num_uuids128 = 1;
    scan_response_fields.uuids128_is_complete = true;
    rc = ble_gap_adv_rsp_set_fields(&scan_response_fields);
    if (rc) {
      ESP_LOGE(TAG,
               "Error while setting additional service 128-bit UUID in GAP "
               "response fields (error %d).",
               rc);
      return;
    }
  }

  if (SERVICE_COUNT_128 > 2) {
    ESP_LOGW(
        TAG,
        "Advertising more than two 128-bit service UUIDs is not supported.");
  }

  // Initialize 16-bit UUID
  fields.num_uuids16 = SERVICE_COUNT_16;
  fields.uuids16 = service_ids_16;
  fields.uuids16_is_complete = 1;

  rc = ble_gap_adv_set_fields(&fields);
  if (rc) {
    ESP_LOGE(TAG,
             "Error while setting first service 16-bit UUID in GAP fields "
             "(error %d).",
             rc);
    return;
  }

  // Advertise
  memset(&adv_params, 0, sizeof adv_params);
  adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
  adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
  rc = ble_gap_adv_start(BLE_OWN_ADDR_PUBLIC, NULL, BLE_HS_FOREVER, &adv_params,
                         ble_gap_event_cb, NULL);
  if (rc) {
    ESP_LOGE(TAG, "Error while starting GAP advertising: %d.", rc);
  }
}

static void ble_on_reset(int reason) { ESP_LOGE(TAG, "Host was reset."); }

static void ble_on_sync(void) {
  ESP_LOGI(TAG, "Host sync.");
  int rc;

  // Configure an address
  if (ble_hs_util_ensure_addr(0)) {
    ESP_LOGE(TAG, "Error while ensuring address.");
    return;
  }

  // No address randomization
  rc = ble_hs_id_infer_auto(0, &own_addr_type);
  if (rc) {
    ESP_LOGE(TAG, "Error determining address type; rc=%d", rc);
    return;
  }

  // Prepare address
  uint8_t addr_val[6] = {0};
  rc = ble_hs_id_copy_addr(own_addr_type, addr_val, NULL);

  // Advertise
  ble_advertise();

  // Scan (central)
  blecent_scan();
}

void ble_host_task(void *param) {
  ESP_LOGI(TAG, "Host Task Started");
  /* This function will return only when nimble_port_stop() is executed */
  nimble_port_run();

  nimble_port_freertos_deinit();
}

void tk_ble_init(void) {

  // NimBLE init
  ESP_ERROR_CHECK(esp_nimble_hci_and_controller_init());
  nimble_port_init();

  // Device name
  uint8_t mac[6];
  char name[32] = {};
  esp_read_mac(mac, ESP_MAC_BT);
  sprintf(name, "%s-%02X%02X%02X", BT_NAME_PREFIX, mac[3], mac[4], mac[5]);

  if (ble_svc_gap_device_name_set(name)) {
    ESP_LOGW(TAG, "Error while setting BT host name.");
  }

  ESP_LOGI(TAG, "Name is \"%s\".", ble_svc_gap_device_name());

  // TX power
  ESP_ERROR_CHECK(esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P9));

  // Host config
  ble_hs_cfg.reset_cb = ble_on_reset;
  ble_hs_cfg.sync_cb = ble_on_sync;
  ble_hs_cfg.gatts_register_cb = tk_gatt_register_cb;
  ble_hs_cfg.store_status_cb = ble_store_util_status_rr;
  ble_hs_cfg.sm_io_cap = BLE_SM_IO_CAP_NO_IO;
  ble_hs_cfg.sm_bonding = true;
  ble_hs_cfg.sm_sc = 1;
  ble_hs_cfg.sm_mitm = 1;
  ble_hs_cfg.sm_our_key_dist =
      BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID;
  ble_hs_cfg.sm_their_key_dist =
      BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID;

  // Peer storage
  int rc = peer_init(MYNEWT_VAL(BLE_MAX_CONNECTIONS), 64, 64, 64);
  assert(rc == 0);

  // GATT initialization (see up)
  if (tk_gatt_init()) {
    ESP_LOGE(TAG, "Unable to start GATT server.");
    abort();
  }
  // Init keystore
  ble_store_config_init();

  // Start task
  nimble_port_freertos_init(ble_host_task);
}