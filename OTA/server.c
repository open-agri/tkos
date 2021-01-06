/**
 * @file server.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Web server for OTA updates.
 * @version 0.1
 * @date 2020-11-27
 *
 *
 */

#include <freertos/FreeRTOS.h>

#include "esp_ota_ops.h"
#include "freertos/event_groups.h"
#include <esp_http_server.h>
#include <esp_log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

#define TAG "OTA server"

httpd_handle_t OTA_server = NULL;
int8_t flash_status = 0;

EventGroupHandle_t reboot_event_group;
const int REBOOT_BIT = BIT0;

/*****************************************************

        systemRebootTask()

        NOTES: This had to be a task because the web page needed
                        an ack back. So i could not call this in the handler

 *****************************************************/
void systemRebootTask(void *parameter) {

  // Init the event group
  reboot_event_group = xEventGroupCreate();

  // Clear the bit
  xEventGroupClearBits(reboot_event_group, REBOOT_BIT);

  for (;;) {
    // Wait here until the bit gets set for reboot
    EventBits_t staBits = xEventGroupWaitBits(reboot_event_group, REBOOT_BIT,
                                              pdTRUE, pdFALSE, portMAX_DELAY);

    // Did portMAX_DELAY ever timeout, not sure so lets just check to be sure
    if ((staBits & REBOOT_BIT) != 0) {
      ESP_LOGI(TAG, "Rebooting after update.");
      vTaskDelay(2000 / portTICK_PERIOD_MS);

      esp_restart();
    }
  }
}

/* Receive .Bin file */
esp_err_t OTA_update_post_handler(httpd_req_t *req) {
  esp_ota_handle_t ota_handle;

  char ota_buff[1024];
  int content_length = req->content_len;
  ESP_LOGI(TAG, "OTA buffer at %p.", ota_buff);
  ESP_LOGI(TAG, "Content length: %d.", content_length);
  int content_received = 0;
  int recv_len = 0;
  bool is_req_body_started = false;
  const esp_partition_t *update_partition =
      esp_ota_get_next_update_partition(NULL);

  // Unsucessful Flashing
  flash_status = -1;

  do {
    /* Read the data for the request */
    recv_len = httpd_req_recv(req, ota_buff, 1024);
    if (recv_len < 0) {
      if (recv_len == HTTPD_SOCK_ERR_TIMEOUT) {
        ESP_LOGW(TAG, "Socket timeout.");
        /* Retry receiving if timeout occurred */
        continue;
      }
      ESP_LOGE(TAG, "OTA error. Data received: %d.", recv_len);
      return ESP_FAIL;
    }

    // ESP_LOGI(TAG, "Received len = %d.", recv_len);
    // for (int j = 0; j < recv_len; j++) {
    //   printf("%c", ota_buff[j]);
    // }
    // printf("\n");

    if (recv_len > 0) {
      if (!is_req_body_started) {
        ESP_ERROR_CHECK(
            esp_ota_begin(update_partition, content_length, &ota_handle));
        ESP_LOGI(TAG, "Writing first block. Start = %p, len = %x.", ota_buff,
                 recv_len);
        esp_ota_write(ota_handle, ota_buff, recv_len);
        content_received += recv_len;
        is_req_body_started = true;
      } else {
        ESP_LOGI(TAG, "Writing block. Start = %p, len = %x.", ota_buff,
                 recv_len);
        esp_ota_write(ota_handle, ota_buff, recv_len);
        content_received += recv_len;
      }
    } else if (recv_len < 0) {
      ESP_LOGE(TAG, "Error: recv data error!");
    }
  } while (recv_len > 0 && content_received < content_length);

  // End response
  // httpd_resp_send_chunk(req, NULL, 0);
  esp_err_t err = esp_ota_end(ota_handle);
  if (err == ESP_OK) {
    // Lets update the partition
    if (esp_ota_set_boot_partition(update_partition) == ESP_OK) {
      const esp_partition_t *boot_partition = esp_ota_get_boot_partition();

      // Webpage will request status when complete
      // This is to let it know it was successful
      flash_status = 1;

      ESP_LOGI(TAG, "Next boot partition subtype %d at offset 0x%x.",
               boot_partition->subtype, boot_partition->address);
      ESP_LOGI(TAG, "Please Restart System...");
      xEventGroupSetBits(reboot_event_group, REBOOT_BIT);
    } else {
      ESP_LOGE(TAG, "Flash error!");
    }

  } else {
    ESP_LOGE(TAG, "OTA end error: %s.", esp_err_to_name(err));
  }

  return ESP_OK;
}

httpd_uri_t OTA_update = {.uri = "/update",
                          .method = HTTP_POST,
                          .handler = OTA_update_post_handler,
                          .user_ctx = NULL};

httpd_handle_t start_OTA_webserver(void) {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();

  // Lets change this from port 80 (default) to 8080
  // config.server_port = 8080;

  // Lets bump up the stack size (default was 4096)
  config.stack_size = 8192;

  // Start the httpd server
  ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);

  if (httpd_start(&OTA_server, &config) == ESP_OK) {
    // Set URI handlers
    ESP_LOGI(TAG, "Registering URI handlers.");
    httpd_register_uri_handler(OTA_server, &OTA_update);
    return OTA_server;
  }

  ESP_LOGI(TAG, "Error starting server.");
  return NULL;
}

void stop_OTA_webserver(httpd_handle_t server) {
  // Stop the httpd server
  httpd_stop(server);
}
