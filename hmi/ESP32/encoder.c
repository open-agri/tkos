#include "esp_log.h"
#include "esp_timer.h"

#include "encoder.h"

#define TAG "encoder"

static volatile int64_t hmi_encoder_last_micros = 0;
static volatile int16_t hmi_encoder_delta = 0;

esp_timer_handle_t hmi_encoder_delayer;

void hmi_encoder_sample()
{
    // Read direction data line
#if ENCODER_INVERT
    int input = !gpio_get_level(HMI_ENCODER_PIN_B);
#else
    int input = gpio_get_level(HMI_ENCODER_PIN_B);
#endif

    // Update delta
    hmi_encoder_delta += (2 * input) - 1;
}

static void IRAM_ATTR hmi_encoder_isr(void *arg)
{
    // Keep only the first edge and ignore next until DEB_US has passed (since the last edge)
    if (esp_timer_get_time() - hmi_encoder_last_micros > HMI_ENCODER_DEB_US)
    {
        // Delay the sampling?
        esp_timer_start_once(hmi_encoder_delayer, HMI_ENCODER_DEL_US);
    }

    hmi_encoder_last_micros = esp_timer_get_time();
}

int16_t hmi_encoder_moves()
{
    int16_t temp = hmi_encoder_delta;
    hmi_encoder_delta = 0;
    return temp;
}

void hmi_encoder_init()
{
    // GPIO setup
    ESP_ERROR_CHECK(gpio_set_direction(HMI_ENCODER_PIN_A, GPIO_MODE_DEF_INPUT));
    ESP_ERROR_CHECK(gpio_set_direction(HMI_ENCODER_PIN_B, GPIO_MODE_DEF_INPUT));

    ESP_ERROR_CHECK(gpio_set_pull_mode(HMI_ENCODER_PIN_A, GPIO_PULLDOWN_ONLY));
    ESP_ERROR_CHECK(gpio_set_pull_mode(HMI_ENCODER_PIN_B, GPIO_PULLDOWN_ONLY));

    // Attach the ISR
    ESP_ERROR_CHECK(gpio_set_intr_type(HMI_ENCODER_PIN_A, GPIO_INTR_POSEDGE));
    ESP_ERROR_CHECK(gpio_isr_handler_add(HMI_ENCODER_PIN_A, hmi_encoder_isr, NULL));

    esp_timer_create_args_t encoder_timer_args = {
        .callback = hmi_encoder_sample,
        .name = "hmi_encoder_delayer"
    };

    esp_timer_create(&encoder_timer_args, &hmi_encoder_delayer);

    ESP_LOGI(TAG, "Encoder initialized");
}

bool hmi_encoder_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    data->enc_diff = hmi_encoder_moves();
    if (data->enc_diff != 0)
        ESP_LOGD(TAG, "Encoder delta: %d", data->enc_diff);

    // Encoder with no button
    data->state = LV_INDEV_STATE_REL;

    return false;
}