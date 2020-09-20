/**
 * @file gap.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Bluetooth LE GAP functions.
 * @version 0.1
 * @date 2020-09-20
 *
 *
 */

#include "gap.h"

#define TAG "BLE GAP"

int tk_ble_gap_start_advertising(void) {
  ESP_LOGI(TAG, "Starting GAP advertising.");
  struct ble_gap_adv_params adv_params;
  struct ble_hs_adv_fields fields;
  const char *name;

  // Fields --------------------------------------------------------
  memset(&fields, 0, sizeof fields);

  // Discoverable and BLE-only
  fields.flags = BLE_HS_ADV_F_BREDR_UNSUP | BLE_HS_ADV_F_DISC_GEN;

  // Enable automatic power info
  fields.tx_pwr_lvl_is_present = true;
  fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;

  // Get name
  name = ble_svc_gap_device_name();
  fields.name_is_complete = 1;
  fields.name_len = strlen(name);
  fields.name = (uint8_t *)name;

  // Set UUIDs
  // fields.uuids...
  int res = ble_gap_adv_set_fields(&fields);
  if (res) {
    ESP_LOGW(TAG, "Error setting BLE GAP fields. Error %d.", res);
    return res;
  }

  // Advertising parameters ----------------------------------------
  memset(&adv_params, 0, sizeof adv_params);

  // Not connectable
  ESP_LOGI(TAG, "Device is set to be %s connectable.", TK_BLE_GAP_CONNECTABLE ? "indirectly" : "not");
  adv_params.conn_mode = TK_BLE_GAP_CONNECTABLE ? BLE_GAP_CONN_MODE_UND : BLE_GAP_CONN_MODE_NON;
  adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

  res = ble_gap_adv_start(BLE_OWN_ADDR_PUBLIC, NULL, BLE_HS_FOREVER,
                          &adv_params, NULL, NULL);

  
  if (res)
    ESP_LOGW(TAG, "Unable to start GAP advertising. Error %d.", res);

  return res;
}