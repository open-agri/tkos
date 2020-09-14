/**
 * @file refresh.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Global refresh signal source.
 * @version 0.1
 * @date 2020-09-11
 * 
 * 
 */

#include <stdlib.h>

#include "ui/views.h"
#include "ui/bars/bars.h"
#include "ui/refresh/refresh.h"

#include "model/datastore.h"

#include "esp_log.h"


#define TAG "Refresher"

/**
 * @brief This is an lvgl task which is the source of the global refresh signal.
 * 
 * @param task Declared because lvgl tasks need this.
 */
void refresher_task(lv_task_t *task)
{
    // TODO: Remove fake data
    global_datastore.engine_data.rpm = abs(global_datastore.engine_data.rpm + (rand() % 6 - 3));
    global_datastore.engine_data.rpm_available = true;
    ESP_LOGD(TAG, "[FAKE DATA] Setting engine RPM to %.2f", global_datastore.engine_data.rpm);

    // Refresh screen
    lv_event_send_refresh_recursive(NULL);
}