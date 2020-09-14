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
#include "esp_err.h"
#include "esp_log.h"

#define TAG "Brightness"

ledc_channel_config_t ledc_channel;
static tk_brightness_settings_t *settings_int;
static bool dark_theme = true;

/**
 * @brief Sets whether the brightness should vary based on current brightness.
 * 
 * @param is_auto Sets the automatic mode to on or off.
 */
void hmi_brightness_set_auto(bool is_auto)
{
}

/**
 * @brief Sets the desired brightness level and switches to manual mode.
 * 
 * @param value Brightness value (0-1).
 */
void hmi_brightness_set_value(double value)
{
}

/**
 * @brief Gets the current automatic contfiguration.
 * 
 * @return true Automatic mode.
 * @return false Manual mode.
 */
bool hmi_brightness_get_auto(void)
{
    return settings_int->automatic;
}

/**
 * @brief Gets the current brightness value, even in automatic mode.
 * 
 * @return unsigned short int The display brightness (0-1).
 */
double hmi_brightness_get_value(void)
{
    return settings_int->level;
}

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

    ESP_LOGI(TAG, "Screen backlight brightness set to duty cycle %d.", duty_cycle);

    ESP_ERROR_CHECK(ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, duty_cycle));
    ESP_ERROR_CHECK(ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel));
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
        .duty = BRIGHTNESS_MAX,
        .gpio_num = BRIGHTNESS_PIN,
        .speed_mode = ledc_timer.speed_mode,
        .hpoint = 0,
        .timer_sel = ledc_timer.timer_num};

    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    // Set up ADC1
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_12Bit));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_0db));

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

    // Redraw screen
    // view_navigate(view_stack_last->generator, false);
}

/**
 * @brief Reads data from the local settings and pushes it to the hardware. Saves updated value to settings.
 * 
 */
void brightness_task(lv_task_t *task)
{
    if (true || settings_int->automatic)
    {

        // Get data from sensor
        int reading = adc1_get_raw(ADC1_CHANNEL_0);
        ESP_LOGI(TAG, "Light sensor raw is %d.", reading);
        if (reading > LIGHT_DARK)
            reading = LIGHT_DARK;
        if (reading < LIGHT_BRIGHT)
            reading = LIGHT_BRIGHT;
        double environment_darkness = (double)(reading - LIGHT_BRIGHT) / (double)(LIGHT_DARK - LIGHT_BRIGHT);
        double environment_light = 1.0 - environment_darkness;

        // Update struct
        settings_int->level = exp_roll_avg(settings_int->level, environment_light);

        // Set display to correct value
        brightness_write(settings_int->level);
    }
    else
    {
        // TODO: Update value to what is stored in settings
    }

    // Update theme (with hysteresis)
    ESP_LOGE(TAG, "level = %f, dark = %d", settings_int->level, dark_theme);
    if (settings_int->level > THEME_THRESHOLD_HIGH && dark_theme)
    {
        ESP_LOGW(TAG, "Switching to light.");
        switch_theme(true);
    }
    else if (settings_int->level < THEME_THRESHOLD_LOW && !dark_theme)
    {
        ESP_LOGW(TAG, "Switching to dark.");
        switch_theme(false);
    }
}