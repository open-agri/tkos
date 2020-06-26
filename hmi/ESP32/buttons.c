#include "esp_log.h"
#include "esp_timer.h"

#include "buttons.h"

#define TAG "buttons"

static volatile uint64_t hmi_button_last_micros = 0;
static volatile int16_t hmi_button_last = -1;
int hmi_button_isr_id = -1;

esp_timer_handle_t hmi_buttons_delayer;

void hmi_buttons_sample(void *arg)
{
    int id = *(int *)arg;
    int pin;

    // Associate id to pin
    switch (id)
    {
    case HMI_BUTTON_ID_LEFT:
        pin = HMI_BUTTON_PIN_LEFT;
        break;
    case HMI_BUTTON_ID_RIGHT:
        pin = HMI_BUTTON_PIN_RIGHT;
        break;
    default:
        pin = -1;
        break;
    }

    // Some time after the interrupt we should see if it was a press or a release
    if (gpio_get_level(pin))
    {
        hmi_button_last = id;
    }
    else
    {
        hmi_button_last = -1;
    }

}

static void IRAM_ATTR hmi_buttons_isr(void *arg)
{
    hmi_button_isr_id = (int)arg;
    if (esp_timer_get_time() - hmi_button_last_micros > HMI_BUTTON_DEB_US)
        // Delay reading
        ESP_ERROR_CHECK(esp_timer_start_once(hmi_buttons_delayer, HMI_BUTTON_DEL_US));

    hmi_button_last_micros = esp_timer_get_time();
}

void hmi_buttons_init()
{
    // GPIO setup
    ESP_ERROR_CHECK(gpio_set_direction(HMI_BUTTON_PIN_LEFT, GPIO_MODE_DEF_INPUT));
    ESP_ERROR_CHECK(gpio_set_direction(HMI_BUTTON_PIN_RIGHT, GPIO_MODE_DEF_INPUT));

    ESP_ERROR_CHECK(gpio_set_pull_mode(HMI_BUTTON_PIN_LEFT, GPIO_PULLDOWN_ONLY));
    ESP_ERROR_CHECK(gpio_set_pull_mode(HMI_BUTTON_PIN_RIGHT, GPIO_PULLDOWN_ONLY));

    // Attach the ISR
    ESP_ERROR_CHECK(gpio_set_intr_type(HMI_BUTTON_PIN_LEFT, GPIO_INTR_ANYEDGE));
    ESP_ERROR_CHECK(gpio_isr_handler_add(HMI_BUTTON_PIN_LEFT, hmi_buttons_isr, (void *)HMI_BUTTON_ID_LEFT));
    ESP_ERROR_CHECK(gpio_set_intr_type(HMI_BUTTON_PIN_RIGHT, GPIO_INTR_ANYEDGE));
    ESP_ERROR_CHECK(gpio_isr_handler_add(HMI_BUTTON_PIN_RIGHT, hmi_buttons_isr, (void *)HMI_BUTTON_ID_RIGHT));

    esp_timer_create_args_t buttons_timer_args = {
        .callback = hmi_buttons_sample,
        .name = "hmi_buttons_delayer",
        .arg = (void *)&hmi_button_isr_id};

    esp_timer_create(&buttons_timer_args, &hmi_buttons_delayer);

    ESP_LOGI(TAG, "Buttons initialized");
}

bool hmi_buttons_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{

    static uint32_t last_btn = 0; /*Store the last pressed button*/
    int btn_pr = hmi_button_last; /*Get the ID (0,1,2...) of the pressed button*/
    if (btn_pr >= 0)
    {                                    /*Is there a button press? (E.g. -1 indicated no button was pressed)*/
        last_btn = btn_pr;               /*Save the ID of the pressed button*/
        data->state = LV_INDEV_STATE_PR; /*Set the pressed state*/
    }
    else
    {
        data->state = LV_INDEV_STATE_REL; /*Set the released state*/
    }

    data->btn_id = last_btn; /*Save the last button*/

    return false;
}
