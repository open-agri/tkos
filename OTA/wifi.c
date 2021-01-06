/**
 * @file wifi.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief WiFi access point for OTA updates.
 * @version 0.1
 * @date 2020-11-27
 *
 *
 */

#include "OTA/server.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "freertos/event_groups.h"
#include <esp_log.h>
#include <mdns.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "model/datastore.h"
#include "ota.h"
#include "wifi.h"

#define TAG "Wi-Fi"

/* FreeRTOS event group to signal when we are connected*/
EventGroupHandle_t wifi_event_group;

const int AP_CLIENT_CONNECTED_BIT = BIT0;
const int AP_CLIENT_DISCONNECTED_BIT = BIT1;
const int WIFI_STA_CONNECTED_BIT = BIT0;
const int WIFI_STA_DISCONNECTED_BIT = BIT1;

static char *rand_string(char *str, size_t size) {
  const char charset[] =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  if (size) {
    for (size_t n = 0; n < size; n++) {
      int key = rand() % (int)(sizeof charset - 1);
      str[n] = charset[key];
    }
    str[size] = '\0';
  }
  return str;
}

static esp_err_t event_handler(void *ctx, system_event_t *event) {
  httpd_handle_t *server = (httpd_handle_t *)ctx;

  switch (event->event_id) {
  case SYSTEM_EVENT_WIFI_READY:
    ESP_LOGI(TAG, "SYSTEM_EVENT_WIFI_READY\r");
    break;
  case SYSTEM_EVENT_SCAN_DONE:
    ESP_LOGI(TAG, "SYSTEM_EVENT_SCAN_DONE\r");
    break;                     /**< ESP32 finish scanning AP */
  case SYSTEM_EVENT_STA_START: /**< ESP32 station start */
    ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START\r");
    esp_wifi_connect();
    break;

  case SYSTEM_EVENT_STA_STOP:
    ESP_LOGI(TAG, "SYSTEM_EVENT_STA_STOP\r");
    break; /**< ESP32 station stop */
  case SYSTEM_EVENT_STA_CONNECTED:
    ESP_LOGI(TAG, "SYSTEM_EVENT_STA_CONNECTED\r");
    break;                            /**< ESP32 station connected to AP */
  case SYSTEM_EVENT_STA_DISCONNECTED: /**< ESP32 station disconnected from AP */
    ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED\r");
    esp_wifi_connect();
    xEventGroupClearBits(wifi_event_group, WIFI_STA_DISCONNECTED_BIT);
    ESP_LOGI(TAG, "retry to connect to the AP\r");
    /* Stop the web server */
    if (*server) {
      stop_OTA_webserver(*server);
      *server = NULL;
    }
    break;

  case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
    ESP_LOGI(TAG, "SYSTEM_EVENT_STA_AUTHMODE_CHANGE\r");
    break; /**< the auth mode of AP connected by ESP32 station changed */
  case SYSTEM_EVENT_STA_GOT_IP: /**< ESP32 station got IP from connected AP */
    ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP\r");
    ESP_LOGI(TAG, "got ip:%s",
             ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
    xEventGroupSetBits(wifi_event_group, WIFI_STA_CONNECTED_BIT);
    /* Start the web server */
    if (*server == NULL) {
      *server = start_OTA_webserver();
    }
    break;

  case SYSTEM_EVENT_STA_LOST_IP:
    ESP_LOGI(TAG, "SYSTEM_EVENT_STA_LOST_IP\r");
    break; /**< ESP32 station lost IP and the IP is reset to 0 */
  case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
    ESP_LOGI(TAG, "SYSTEM_EVENT_STA_WPS_ER_SUCCESS\r");
    break; /**< ESP32 station wps succeeds in enrollee mode */
  case SYSTEM_EVENT_STA_WPS_ER_FAILED:
    ESP_LOGI(TAG, "SYSTEM_EVENT_STA_WPS_ER_FAILED\r");
    break; /**< ESP32 station wps fails in enrollee mode */
  case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
    ESP_LOGI(TAG, "SYSTEM_EVENT_STA_WPS_ER_TIMEOUT\r");
    break; /**< ESP32 station wps timeout in enrollee mode */
  case SYSTEM_EVENT_STA_WPS_ER_PIN:
    ESP_LOGI(TAG, "SYSTEM_EVENT_STA_WPS_ER_PIN\r");
    break; /**< ESP32 station wps pin code in enrollee mode */
  case SYSTEM_EVENT_AP_START:
    ESP_LOGI(TAG, "SYSTEM_EVENT_AP_START\r");
    break; /**< ESP32 soft-AP start */
  case SYSTEM_EVENT_AP_STOP:
    ESP_LOGI(TAG, "SYSTEM_EVENT_AP_STOP\r");
    break;                           /**< ESP32 soft-AP stop */
  case SYSTEM_EVENT_AP_STACONNECTED: /**< a station connected to ESP32 soft-AP
                                      */
    ESP_LOGI(TAG, "SYSTEM_EVENT_AP_STACONNECTED\r");

    ESP_LOGI(TAG, "station:" MACSTR " join, AID=%d",
             MAC2STR(event->event_info.sta_connected.mac),
             event->event_info.sta_connected.aid);
    xEventGroupSetBits(wifi_event_group, AP_CLIENT_CONNECTED_BIT);
    /* Start the web server */
    if (*server == NULL) {
      *server = start_OTA_webserver();
    }
    break;

  case SYSTEM_EVENT_AP_STADISCONNECTED: /**< a station disconnected from ESP32
                                           soft-AP */
    ESP_LOGI(TAG, "SYSTEM_EVENT_AP_STADISCONNECTED\r");
    ESP_LOGI(TAG, "station:" MACSTR "leave, AID=%d",
             MAC2STR(event->event_info.sta_disconnected.mac),
             event->event_info.sta_disconnected.aid);
    xEventGroupSetBits(wifi_event_group, AP_CLIENT_DISCONNECTED_BIT);
    /* Stop the web server */
    if (*server) {
      stop_OTA_webserver(*server);
      *server = NULL;
    }
    break;

  case SYSTEM_EVENT_AP_PROBEREQRECVED:
    ESP_LOGI(TAG, "SYSTEM_EVENT_AP_PROBEREQRECVED\r");
    break; /**< Receive probe request packet in soft-AP interface */
  case SYSTEM_EVENT_GOT_IP6:
    ESP_LOGI(TAG, "SYSTEM_EVENT_GOT_IP6\r");
    break; /**< ESP32 station or ap or ethernet interface v6IP addr is preferred
            */
  case SYSTEM_EVENT_ETH_START:
    ESP_LOGI(TAG, "SYSTEM_EVENT_ETH_START\r");
    break; /**< ESP32 ethernet start */
  case SYSTEM_EVENT_ETH_STOP:
    ESP_LOGI(TAG, "SYSTEM_EVENT_ETH_STOP\r");
    break; /**< ESP32 ethernet stop */
  case SYSTEM_EVENT_ETH_CONNECTED:
    ESP_LOGI(TAG, "SYSTEM_EVENT_ETH_CONNECTED\r");
    break; /**< ESP32 ethernet phy link up */
  case SYSTEM_EVENT_ETH_DISCONNECTED:
    ESP_LOGI(TAG, "SYSTEM_EVENT_ETH_DISCONNECTED\r");
    break; /**< ESP32 ethernet phy link down */
  case SYSTEM_EVENT_ETH_GOT_IP:
    ESP_LOGI(TAG, "SYSTEM_EVENT_ETH_GOT_IP\r");
    break; /**< ESP32 ethernet got IP from connected AP */
  case SYSTEM_EVENT_MAX:
    ESP_LOGI(TAG, "SYSTEM_EVENT_MAX\r");
    break;
  default:
    ESP_LOGI(TAG, "SYSTEM_EVENT_OTHER\r");
    break;
  }

  return ESP_OK;
}

void start_dhcp_server(void) {

  // initialize the tcp stack
  tcpip_adapter_init();

  // stop DHCP server
  ESP_ERROR_CHECK(tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP));

  // assign a static IP to the network interface
  tcpip_adapter_ip_info_t info;
  memset(&info, 0, sizeof(info));
  IP4_ADDR(&info.ip, 192, 168, 1, 1);
  IP4_ADDR(&info.gw, 192, 168, 1, 1);
  IP4_ADDR(&info.netmask, 255, 255, 255, 0);
  ESP_ERROR_CHECK(tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &info));

  // start the DHCP server
  ESP_ERROR_CHECK(tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP));
  ESP_LOGI(TAG, "DHCP server started ");
}

void start_mdns() {
  esp_err_t err = mdns_init();
  if(err) {
    ESP_LOGE(TAG, "Cannot start mDNS service.");
    return;
  }

  mdns_hostname_set("openagri-ota");
  mdns_instance_name_set("Commander");
  mdns_service_add(NULL, "_http", "_tcp", 80, NULL, 0);
}

void wifi_init_softap(void *arg) {

  // Random seed
  time_t t;
  srand((unsigned)time(&t));

  ESP_LOGI(TAG, "Starting SoftAP for OTA.");

  start_dhcp_server();

  wifi_event_group = xEventGroupCreate();

  tcpip_adapter_init();
  ESP_ERROR_CHECK(esp_event_loop_init(event_handler, arg));

  start_mdns();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

  // SSID
  char ssid[17];
  char r[13];
  rand_string(r, 12);
  sprintf(ssid, "OTA-%s", r);

  strncpy(global_datastore.wifi_settings.ssid, ssid,
          sizeof(global_datastore.wifi_settings.ssid));

  //   char password[17];
  //   rand_string(password, 16);
  //   strncpy(global_datastore.wifi_settings.password, password,
  //           sizeof(global_datastore.wifi_settings.password));

  wifi_config_t ap_config = {
      .ap =
          {
              .ssid_len = 16,
              .channel = 0,
              .authmode = WIFI_AUTH_OPEN,
              .ssid_hidden = false, // TODO: Change to true
              .max_connection = 1,
              .beacon_interval = 100,
          },
  };

  memcpy(ap_config.ap.ssid, (uint8_t *)ssid, 16);
  // memcpy(ap_config.ap.password, (uint8_t *)password, 16);
  // ESP_LOGI(TAG, "SSID: %s, PASS: %s", ssid, password);

  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_ERROR_CHECK(esp_wifi_set_max_tx_power(8));

  global_datastore.wifi_settings.ap_enable = true;

  ESP_LOGI(TAG, "SoftAP started.");
}

void wifi_stop_softap(void *arg) {
  ESP_LOGW(TAG, "Stopping OTA AP is not supported.");
  // ESP_ERROR_CHECK(esp_wifi_stop());
  // ESP_ERROR_CHECK(esp_wifi_deinit());

  // global_datastore.wifi_settings.ap_enable = false;

  // ESP_LOGI(TAG, "SoftAP stopped.");
}
