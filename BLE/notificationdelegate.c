/**
 * @file notificationdelegate.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief GATT notification data handler.
 * @version 0.1
 * @date 2020-12-05
 *
 *
 */

#include "BLE/notificationdelegate.h"
#include "model/datastore.h"

#include <math.h>
#include <sys/time.h>
#include <time.h>

#define TAG "GATT notification delegate"

// Setting handle to -1 means that it's not subscribed yet.
tk_ble_notification_identifier_t
    interesting_notifications[NUM_INTERESTING_NOTIFICATIONS] = {
        {-1, &(tk_id_engine_rpm.u), &(tk_id_engine_rpm_ch_rpm.u)},
        {-1, &(tk_id_engine_temperature.u),
         &(tk_id_engine_temperature_ch_engine.u)},
        {-1, &(tk_id_location.u), &(tk_id_location_ch_gps_avail.u)},
        {-1, &(tk_id_location.u), &(tk_id_location_ch_timestamp.u)},
        {-1, &(tk_id_location.u), &(tk_id_location_ch_speed_kph.u)},
        {-1, &(tk_id_engine_temperature.u), &(tk_id_engine_temperature_ch_engine.u)}};

void tk_ble_rpm_recv(struct os_mbuf *om);
// void tk_ble_rpm_avail_recv(struct os_mbuf *om);
void tk_ble_temperature_recv(struct os_mbuf *om);
void tk_ble_gps_avail_recv(struct os_mbuf *om, int conn_handle);
void tk_ble_gps_speed_kph_recv(struct os_mbuf *om);

static int tk_om_decode(struct os_mbuf *om, uint16_t min_len, uint16_t max_len,
                        void *dst, uint16_t *len) {
  uint16_t om_len;
  int rc;

  om_len = OS_MBUF_PKTLEN(om);
  if (om_len < min_len || om_len > max_len) {
    return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
  }

  rc = ble_hs_mbuf_to_flat(om, dst, max_len, len);
  if (rc != 0) {
    return BLE_ATT_ERR_UNLIKELY;
  }

  return 0;
}

void tk_ble_handle_gatt_notification(uint16_t conn_handle, uint16_t attr_handle,
                                     struct os_mbuf *om) {

  ESP_LOGV(TAG, "Handling conn %d, attr %d.", conn_handle, attr_handle);

  // Find val_handle
  int i = -1;
  for (i = 0; i < NUM_INTERESTING_NOTIFICATIONS; i++) {
    if (interesting_notifications[i].val_handle == attr_handle) {
      // Found
      break;
    }
  }

  // Not found
  if (i == -1 || i == NUM_INTERESTING_NOTIFICATIONS)
    return;

  ble_uuid_t *chr_id = interesting_notifications[i].chr_id;

  // Dispatch
  if (ble_uuid_cmp(chr_id, &(tk_id_engine_rpm_ch_rpm.u)) == 0) {
    tk_ble_rpm_recv(om);
  } else if (ble_uuid_cmp(chr_id, &(tk_id_location_ch_speed_kph.u)) == 0) {
    tk_ble_gps_speed_kph_recv(om);
  } else if (ble_uuid_cmp(chr_id, &(tk_id_location_ch_gps_avail.u)) == 0) {
    tk_ble_gps_avail_recv(om, conn_handle);
  } else if (ble_uuid_cmp(chr_id, &(tk_id_engine_temperature_ch_engine.u)) == 0) {
    tk_ble_temperature_recv(om);
  }
}

void tk_ble_temperature_recv(struct os_mbuf *om) {
  float temp;
  tk_om_decode(om, sizeof temp, sizeof temp, &temp, NULL);

  global_datastore.engine_data.temp_c = temp;
  global_datastore.engine_data.temp_c_available = (temp != 0.0/0.0);

  ESP_LOGD(TAG, "Temerature received: %.2f.", temp);
}

void tk_ble_rpm_recv(struct os_mbuf *om) {
  double rpm;
  tk_om_decode(om, sizeof rpm, sizeof rpm, &rpm, NULL);

  global_datastore.engine_data.rpm_available = (rpm > 0.0);
  global_datastore.engine_data.rpm = rpm;

  ESP_LOGD(TAG, "RPM received: %.2f.", rpm);
}

void tk_ble_gps_speed_kph_recv(struct os_mbuf *om) {
  double speed_kph;
  tk_om_decode(om, sizeof speed_kph, sizeof speed_kph, &speed_kph, NULL);

  global_datastore.location_data.speed = speed_kph;

  ESP_LOGD(TAG, "Speed received: %.2f km/h.", speed_kph);
}

int time_received(uint16_t conn_handle, const struct ble_gatt_error *error,
                  struct ble_gatt_attr *attr, void *arg) {
  time_t epoch;
  if (attr == NULL || attr->om == NULL) {
    ESP_LOGE(TAG, "Received null epoch.");
    if (error != NULL) {
      ESP_LOGE(TAG, "Error was %d.", error->status);
    }
    return 1;
  }

  tk_om_decode(attr->om, sizeof epoch, sizeof epoch, &epoch, NULL);

  ESP_LOGI(TAG, "Epoch received: %ld.", epoch);

  // Set time
  struct timeval now;
  now.tv_sec = epoch;
  now.tv_usec = 0;

  settimeofday(&now, NULL);

  setenv("TZ", "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00", 1);
  tzset();

  ESP_LOGI(TAG, "Time set.");

  return 0;
}

void tk_ble_gps_avail_recv(struct os_mbuf *om, int conn_handle) {
  bool gps_avail;
  tk_om_decode(om, sizeof gps_avail, sizeof gps_avail, &gps_avail, NULL);

  // On rise, update date/time
  if (gps_avail == true &&
      global_datastore.gps_status != TK_GPS_STATUS_CONNECTED) {
    ESP_LOGI(TAG, "Getting date/time from GPS device.");

    int rc =
        ble_gattc_read(conn_handle, interesting_notifications[3].val_handle,
                       time_received, NULL);

    if (rc != 0) {
      ESP_LOGE(TAG, "Error while requesting time data: %d.", rc);
    }
  }

  global_datastore.location_data.speed_available = gps_avail;
  global_datastore.gps_status =
      gps_avail ? TK_GPS_STATUS_CONNECTED : TK_GPS_STATUS_CONNECTING;

  ESP_LOGD(TAG, "GPS availability received: %d.", gps_avail);
}
