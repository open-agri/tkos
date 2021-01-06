#pragma once

// FreeRTOS includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"

#include "ui/views.h"

// Definitions
#define LV_TICK_PERIOD_MS 5
#define LV_THEME_LIVE_UPDATE 1

// Global variables
lv_indev_t *encoder_indev;

// Prototypes

/**
 * @brief The starting point for tkos. The main task for UI-related jobs.
 * 
 * @param pvParameter 
 */
void guiTask(void *pvParameter);
