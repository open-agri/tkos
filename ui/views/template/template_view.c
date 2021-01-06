/**
 * @file template_view.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief The template view builder.
 * @version 0.1
 * @date 2020-09-10
 * 
 * 
 */

#include "ui/views.h"
#include "esp_log.h"

#include <stdio.h>

#define TAG "Template view"

/**
 * @brief The bottom bar's left button click callback.
 * 
 */
static void left_button_click_callback()
{
  ESP_LOGI(TAG, "Left button pressed.");
  // view_navigate_back();
}

/**
 * @brief The bottom bar's right button click callback.
 * 
 */
static void right_button_click_callback()
{
  ESP_LOGI(TAG, "Right button pressed.");
}

/**
 * @brief The template view generator.
 * 
 * @return tk_view_t The generated view.
 */
tk_view_t build_template_view()
{

  ESP_LOGI(TAG, "Building view.");

  // Content
  lv_obj_t *view_content = lv_cont_create(NULL, NULL);
  lv_obj_add_style(view_content, LV_CONT_PART_MAIN, &tk_style_far_background);

  // Group (for encoder)
  lv_group_t *group = lv_group_create();

  // Bottom bar configuration
  tk_bottom_bar_button_t right_bar_button = {
      .text = "Button",
      .click_callback = right_button_click_callback,
      .menu = {(tk_bar_menu_item_t){.text = "First", .click_callback = NULL},
               (tk_bar_menu_item_t){.text = "Second", .click_callback = NULL}},
      .items_count = 2};

  tk_bottom_bar_button_t left_bar_button = {
      .text = LV_SYMBOL_LEFT "   Back",
      .click_callback = left_button_click_callback};

  tk_bottom_bar_configuration_t bb_conf = {
      .right_button = right_bar_button,
      .left_button = left_bar_button};

  // Return struct
  tk_view_t main_view = {
      .content = view_content,
      .bottom_bar_configuration = bb_conf};

  ESP_LOGD(TAG, "View built successfully.");

  return main_view;
}
