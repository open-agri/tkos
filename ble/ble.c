/**
 * @file ble.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Main file for Bluetooth LE connection.
 * @version 0.1
 * @date 2020-09-19
 *
 *
 */

#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"

#include "ble.h"

#include "esp_nimble_hci.h"
#include "host/ble_hs.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "services/gap/ble_svc_gap.h"

#define TAG "BLE"

/**
 * @brief FreeRTOS task for BLE.
 *
 * @param param
 */
void ble_central_task(void *param) {
  ESP_LOGI(TAG, "BLE Host Task Started");
  // This function will return only when nimble_port_stop() is executed
  nimble_port_run();

  nimble_port_freertos_deinit();
}

/**
 * @brief Gets called when host and controller are in sync. After that, BLE can
 * be used.
 *
 */
void ble_central_sync_cb(void) {
  ESP_LOGI(TAG, "Host and controller in sync.");
}

/**
 * @brief Gets called when a catastrophic error occurs or host and controller
 * fail to communicate.
 *
 * @param reason
 */
void ble_central_reset_cb(int reason) {
  ESP_LOGE(TAG, "Host was reset, reason %d.", reason);
}

void ble_central_init() {
  ESP_LOGI(TAG, "Initializing BLE central. Free memory: %d.",
           esp_get_free_heap_size());

  // Initialize controller and NimBLE
  // ESP_ERROR_CHECK(esp_nimble_hci_and_controller_init());
  int ret = esp_nimble_hci_and_controller_init();
  if (ret != ESP_OK) {
    ESP_LOGE(TAG,
             "esp_nimble_hci_and_controller_init() failed with error \"%s\".",
             esp_err_to_name(ret));
    return;
  }

  ESP_LOGI(TAG, "Initializing NimBLE.");
  nimble_port_init();

  // Host configuration
  ble_hs_cfg.reset_cb = ble_central_reset_cb;
  ble_hs_cfg.sync_cb = ble_central_sync_cb;

  // Device name
  uint8_t mac[6];
  char name[32] = {};
  esp_read_mac(mac, ESP_MAC_BT);

  sprintf(name, "TKC-%02X%02X%02X", mac[3], mac[4], mac[5]);
  ESP_LOGI(TAG, "Setting device Bluetooth name to \"%s\".", name);

  if (!ble_svc_gap_device_name_set(name))
    ESP_LOGE(TAG, "Unable to change device name.");

  // Store config
  // ble_store_config_init();

  nimble_port_freertos_init(ble_central_task);
}