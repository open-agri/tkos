/**
 * @file boot_view.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief The boot view builder.
 * @version 0.1
 * @date 2020-12-05
 * 
 * 
 */

#include "ui/views.h"
#include "esp_log.h"

#include <stdio.h>

#define TAG "Boot view"

/**
 * @brief The boot view generator.
 * 
 * @return tk_view_t The generated view.
 */
tk_view_t build_boot_view()
{

  ESP_LOGI(TAG, "Building view.");

  // Content
  lv_obj_t *view_content = lv_cont_create(NULL, NULL);
  lv_obj_add_style(view_content, LV_CONT_PART_MAIN, &tk_style_far_background);

  // Group (for encoder)
  lv_group_t *group = lv_group_create();

  // Bottom bar configuration
  tk_bottom_bar_configuration_t bb_conf = {};

  // Return struct
  tk_view_t main_view = {
      .content = view_content,
      .bottom_bar_configuration = bb_conf};

  ESP_LOGD(TAG, "View built successfully.");

  return main_view;
}
