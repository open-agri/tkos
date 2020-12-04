/**
 * @file brightness.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Screen brightness management.
 * @version 0.1
 * @date 2020-09-13
 * 
 * 
 */

#include <stdbool.h>

#include "ui/styles/tk_style.h"
#include "ui/views.h"

#include "lvgl/lvgl.h"
#include "lvgl/src/lv_themes/lv_theme_material.h"

#include "hmi/ESP32/brightness.h"
#include "driver/ledc.h"
#include "driver/adc.h"
#include "soc/adc_channel.h"
#include "esp_err.h"
#include "esp_log.h"

#define TAG "Brightness"

ledc_channel_config_t ledc_channel;
static tk_brightness_settings_t *settings_int;
static bool dark_theme = true;

/**
 * @brief Writes the brightness value to the backlight.
 * 
 * @param value Brightness value (0-1).
 */
void brightness_write(double value)
{
    // TODO: Implement fade
    // Constrain input
    if (value < 0)
        value = 0;
    if (value > 1)
        value = 1;

    // Get duty cycle
    unsigned int duty_cycle = BRIGHTNESS_MIN + (value * (double)(BRIGHTNESS_MAX - BRIGHTNESS_MIN));

    ESP_ERROR_CHECK(ledc_set_fade_time_and_start(ledc_channel.speed_mode, ledc_channel.channel, duty_cycle, 100, LEDC_FADE_NO_WAIT));
}

/**
 * @brief Initializes the brightness manager.
 * 
 * @param settings Initial settings.
 */
void hmi_brightness_init(tk_brightness_settings_t *settings)
{
    // Save the pointer for internal editing
    settings_int = settings;

    // Set up the PWM driver
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_10_BIT,
        .freq_hz = 8000,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .clk_cfg = LEDC_AUTO_CLK};

    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel = (ledc_channel_config_t){
        .channel = LEDC_CHANNEL_0,
        // Default duty to max for diagnostic reasons
        .duty = 0,
        .gpio_num = BRIGHTNESS_PIN,
        .speed_mode = ledc_timer.speed_mode,
        .hpoint = 0,
        .timer_sel = ledc_timer.timer_num};

    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    ESP_ERROR_CHECK(ledc_fade_func_install(0));

    // Set up LDR switcher
    gpio_config_t switcher_gpio = {
        .mode = GPIO_MODE_OUTPUT_OD,
        .intr_type = GPIO_INTR_DISABLE,
        .pin_bit_mask = GPIO_SEL_32};

    ESP_ERROR_CHECK(gpio_config(&switcher_gpio));

    // Disable LDR
    ESP_ERROR_CHECK(gpio_set_level(GPIO_NUM_32, 0));

    // Set up ADC1
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_12Bit));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_GPIO32_CHANNEL, ADC_ATTEN_11db));

    brightness_write(1);//settings_int->level);
    switch_theme(false);

    ESP_LOGI(TAG, "Brightness system initialized.");
}

/**
 * @brief Calculates an exponential rolling average.
 * 
 * @param avg The old average.
 * @param new_sample The new sample.
 * @return double The new average.
 */
double exp_roll_avg(double avg, double new_sample)
{

    avg -= avg / 10;
    avg += new_sample / 10;

    return avg;
}

/**
 * @brief Switches the main theme.
 * 
 * @param light true for material light, false for material dark.
 */
void switch_theme(bool light)
{
    dark_theme = !light;

    // Set theme
    if (light)
    {
        lv_theme_material_init(tk_get_primary_color(true), tk_get_secondary_color(true),
                               LV_THEME_MATERIAL_FLAG_LIGHT,
                               &nunito_bold_12, &nunito_bold_16, &nunito_bold_24, &nunito_bold_36);
    }
    else
    {
        lv_theme_material_init(tk_get_primary_color(false), tk_get_secondary_color(false),
                               LV_THEME_MATERIAL_FLAG_DARK,
                               &nunito_bold_12, &nunito_bold_16, &nunito_bold_24, &nunito_bold_36);
    }

    // Generate styles
    tk_styles_init(light);
}

/**
 * @brief Reads data from the local settings and pushes it to the hardware. Saves updated value to settings.
 * 
 */
void brightness_task(lv_task_t *task)
{
    if (settings_int->automatic)
    {

        // Turn on sensor
        // ESP_ERROR_CHECK(gpio_set_level(GPIO_NUM_32, 1));

        // Get data from sensor
        int reading = adc1_get_raw(ADC1_GPIO32_CHANNEL);

        // Turn off sensor

        // Process reading
        if (reading > LIGHT_DARK)
            reading = LIGHT_DARK;
        if (reading < LIGHT_BRIGHT)
            reading = LIGHT_BRIGHT;
        double environment_darkness = (double)(reading - LIGHT_BRIGHT) / (double)(LIGHT_DARK - LIGHT_BRIGHT);
        double environment_light = 1.0 - environment_darkness;

        // Update struct
        settings_int->level = exp_roll_avg(settings_int->level, environment_light);
        ESP_LOGD(TAG, "Reading = %d. Relative brightness = %.4f, average %.4f.", reading, environment_light, settings_int->level);

        // Set display to correct value
        brightness_write(settings_int->level);
    }
    else
    {
        // Update value to what is stored in settings
        brightness_write(settings_int->level);
    }

    // Update theme (with hysteresis)
    if (settings_int->level > THEME_THRESHOLD_HIGH && dark_theme)
    {
        ESP_LOGI(TAG, "Switching to light.");
        switch_theme(true);
    }
    else if (settings_int->level < THEME_THRESHOLD_LOW && !dark_theme)
    {
        ESP_LOGI(TAG, "Switching to dark.");
        switch_theme(false);
    }
}