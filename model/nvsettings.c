/**
 * @file nvsettings.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Non-volatile settings system
 * @version 0.1
 * @date 2020-11-26
 *
 *
 */

#include "nvsettings.h"

#include "esp_log.h"
#include "nvs_flash.h"

#include "datastore.h"

#define TAG "NV Settings"

nvs_handle_t nv_handle;

// -------------------- GENERAL FUNCTIONS --------------------
void nv_init() {

  ESP_LOGI(TAG, "Initializing NVS.");
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  ESP_LOGI(TAG, "Opening per_settings namespace.");
  esp_err_t err = nvs_open("per_settings", NVS_READWRITE, &nv_handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to initialize NVS for peripheral settings.");
    ESP_ERROR_CHECK(err);
  }
}

void nv_load_settings() {
  ESP_LOGI(TAG, "Loading all non-volatile peripheral settings.");

  // Brightness
  global_datastore.brightness_settings.automatic = nv_get_brightness_auto();
  global_datastore.brightness_settings.level = nv_get_brightness_man_level();
}

void nv_load_apply_settings() {
  ESP_LOGI(TAG, "Loading and applying all non-volatile peripheral settings.");
  nv_load_settings();

  // Brightness
  // Nothing to apply
}

// -------------------- SETTINGS --------------------

// Automatic brightness
void nv_set_brightness_auto(bool automatic) {
  ESP_LOGI(TAG, "Writing and applying automatic brightness setting to %d.", automatic);
  global_datastore.brightness_settings.automatic = automatic;

  // Save
  esp_err_t err = nvs_set_i8(nv_handle, "bri_auto", (int8_t)automatic);

  // Apply
  // Not needed
}

bool nv_get_brightness_auto() {

  int8_t automatic = 1;
  esp_err_t err = nvs_get_i8(nv_handle, "bri_auto", &automatic);
  assert(err == ESP_OK || err == ESP_ERR_NVS_NOT_FOUND);
  if (err == ESP_ERR_NOT_FOUND) {
    ESP_LOGW(TAG,
             "Automatic brightness key not found. Returning default value of 1.");
  }

  ESP_LOGI(TAG, "Automatic brightness setting read from NVS: %d.", automatic);

  return (bool)automatic;
}

// Manual brightness level
void nv_set_brightness_man_level(double level) {
  ESP_LOGI(TAG, "Writing and applying manual brightness level setting to %.2f.", level);
  global_datastore.brightness_settings.level = level;

  // Save
  esp_err_t err = nvs_set_i32(nv_handle, "bri_level", (int32_t)(level * 1000000));

  // Apply
  // Not needed
}

double nv_get_brightness_man_level() {

  int32_t level = 1000000;
  esp_err_t err = nvs_get_i32(nv_handle, "bri_level", &level);
  assert(err == ESP_OK || err == ESP_ERR_NVS_NOT_FOUND);
  if (err == ESP_ERR_NOT_FOUND) {
    ESP_LOGW(TAG,
             "Manual brightness level key not found. Returning default value of 1.");
  }

  ESP_LOGI(TAG, "Manual brightness level setting read from NVS: %.2f.", (double)level/1000000.0);

  return (double)level/1000000.0;
}
