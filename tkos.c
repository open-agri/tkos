#include "tkos.h"

// LVGL includes
#include "lvgl/lvgl.h"

#ifndef SIMULATOR
#include "lvgl/lvgl_helpers.h"

#include "hmi/ESP32/encoder.h"
#include "hmi/ESP32/buttons.h"
#endif

#include "views/views.h"
#include "views/styles/tk_style.h"

void tkos_init(void) {
  tk_styles_init();
}

#ifndef SIMULATOR
static void lv_tick_task(void *arg)
{
    (void)arg;

    lv_tick_inc(LV_TICK_PERIOD_MS);
}

//Creates a semaphore to handle concurrent call to lvgl stuff
//If you wish to call *any* lvgl function from other threads/tasks
//you should lock on the very same semaphore!
SemaphoreHandle_t xGuiSemaphore;

void guiTask(void *pvParameter)
{

    (void)pvParameter;
    xGuiSemaphore = xSemaphoreCreateMutex();

    lv_init();

    /* Initialize SPI or I2C bus used by the drivers */
    lvgl_driver_init();

    static lv_color_t buf1[DISP_BUF_SIZE];
#ifndef CONFIG_LVGL_TFT_DISPLAY_MONOCHROME
    static lv_color_t buf2[DISP_BUF_SIZE];
#endif
    static lv_disp_buf_t disp_buf;

    uint32_t size_in_px = DISP_BUF_SIZE;
    lv_disp_buf_init(&disp_buf, buf1, buf2, size_in_px);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = disp_driver_flush;

    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    // ISR install
    ESP_ERROR_CHECK(gpio_install_isr_service(0));

    // Encoder initialization
    hmi_encoder_init();
    lv_indev_drv_t encoder_drv;
    lv_indev_drv_init(&encoder_drv);
    encoder_drv.type = LV_INDEV_TYPE_ENCODER;
    encoder_drv.read_cb = hmi_encoder_read;
    lv_indev_t *encoder_indev = lv_indev_drv_register(&encoder_drv);

    // Buttons initialization
    hmi_buttons_init();
    lv_indev_drv_t buttons_drv;
    lv_indev_drv_init(&buttons_drv);
    buttons_drv.type = LV_INDEV_TYPE_BUTTON;
    buttons_drv.read_cb = hmi_buttons_read;
    lv_indev_t *buttons_indev = lv_indev_drv_register(&buttons_drv);

    static const lv_point_t points_array[] = {{5, 315}, {475, 315}};
    lv_indev_set_button_points(buttons_indev, points_array);

    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"};

    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    // GUI Start
    tkos_init();
    view_navigate(build_main_view, true);

    while (1)
    {
        vTaskDelay(1);
        //Try to lock the semaphore, if success, call lvgl stuff
        if (xSemaphoreTake(xGuiSemaphore, (TickType_t)10) == pdTRUE)
        {
            lv_task_handler();
            xSemaphoreGive(xGuiSemaphore);
        }
    }

    //A task should NEVER return
    vTaskDelete(NULL);
}
#endif
