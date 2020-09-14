/**
 * @file brightness_view.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief The brightness view builder.
 * @version 0.1
 * @date 2020-09-10
 * 
 * 
 */

#include "ui/views.h"
#include "esp_log.h"

#include <stdio.h>

#define TAG "Brightness view"

lv_group_t *group;
char lb_string[30];
char rb_string[30];

/**
 * @brief The bottom bar's left button click callback.
 * 
 */
static void left_button_click_callback()
{
  ESP_LOGI(TAG, "Left button pressed.");
  view_navigate_back();
}

/**
 * @brief The bottom bar's right button click callback.
 * 
 */
static void right_button_click_callback()
{
  // TODO: If button toggle without entering edit mode.
  // lv_obj_get_type...
  ESP_LOGI(TAG, "Right button pressed.");
  if (lv_group_get_editing(group))
  {
    lv_group_set_editing(group, false);
    strcpy(rb_string, "Edit   " LV_SYMBOL_EDIT);
  }
  else
  {
    lv_group_set_editing(group, true);
    strcpy(rb_string, "Confirm   " LV_SYMBOL_OK);
  }

  lv_event_send_refresh_recursive(lv_scr_act());
}

/**
 * @brief The brightness view generator.
 * 
 * @return tk_view_t The generated view.
 */
tk_view_t build_brightness_view()
{

  ESP_LOGI(TAG, "Building view.");

  // Strings
  strcpy(lb_string, LV_SYMBOL_LEFT "   Back");
  strcpy(rb_string, "Edit   " LV_SYMBOL_EDIT);

  // Content
  lv_obj_t *view_content = lv_cont_create(NULL, NULL);
  lv_obj_add_style(view_content, LV_CONT_PART_MAIN, &tk_style_far_background);

  // Group (for encoder)
  group = lv_group_create();

  // Slider
  lv_obj_t *slider = lv_slider_create(view_content, NULL);
  lv_obj_align(slider, view_content, LV_ALIGN_CENTER, 0, 0);
  lv_group_add_obj(group, slider);

  // Switch
  lv_obj_t *auto_switch = lv_switch_create(view_content, NULL);
  lv_obj_align(auto_switch, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
  lv_group_add_obj(group, auto_switch);

  // Group
  lv_indev_set_group(encoder_indev, group);
  // lv_group_focus_obj(slider);
  // lv_group_set_editing(group, true);

  // Bottom bar configuration
  tk_bottom_bar_button_t right = {
      .text = rb_string,
      .click_callback = right_button_click_callback};

  tk_bottom_bar_button_t left = {
      .text = lb_string,
      .click_callback = left_button_click_callback};

  tk_bottom_bar_configuration_t bb_conf = {
      .right_button = right,
      .left_button = left};

  // Return struct
  tk_view_t main_view = {
      .content = view_content,
      .bottom_bar_configuration = bb_conf};

  ESP_LOGD(TAG, "View built successfully.");

  return main_view;
}
