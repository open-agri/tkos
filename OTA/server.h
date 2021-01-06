/**
 * @file server.h
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Web server for OTA updates.
 * @version 0.1
 * @date 2020-11-27
 * 
 * 
 */

#pragma once

#include <esp_http_server.h>

extern httpd_handle_t OTA_server;

void systemRebootTask(void * parameter);
	
httpd_handle_t start_OTA_webserver(void);
void stop_OTA_webserver(httpd_handle_t server);