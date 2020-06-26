#ifndef TKOS_H
#define TKOS_H

// FreeRTOS includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"

// Definitions
#define LV_TICK_PERIOD_MS 5

// Prototypes
static void lv_tick_task(void *arg);
void guiTask(void *pvParameter);

#endif