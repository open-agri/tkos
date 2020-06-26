#ifndef TKOS_H
#define TKOS_H

#define SIMULATOR

// FreeRTOS includes
#ifndef SIMULATOR
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#endif

#include "views/views.h"

// Definitions
#define LV_TICK_PERIOD_MS 5

// Prototypes
static void lv_tick_task(void *arg);
void guiTask(void *pvParameter);
void tkos_init(void);

#endif