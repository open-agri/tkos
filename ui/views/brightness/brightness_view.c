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
#include "ui/menu/menu.h"
#include "model/datastore.h"
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
  // TODO: If switch toggle without entering edit mode.
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

  lv_obj_t *menu_container = lv_cont_create(view_content, NULL);
  lv_obj_add_style(menu_container, LV_CONT_PART_MAIN, &tk_style_far_background);
  lv_obj_set_size(menu_container, 480, 320 - 2 * 36);
  lv_obj_align(menu_container, NULL, LV_ALIGN_CENTER, 0, 0);

  // Group (for encoder)
  group = lv_group_create();

  // Menu
  static tk_menu_item_t menu_items[] = {
      (tk_menu_item_t){.type = TK_MENU_ITEM_SWITCH, .desc = "Automatic brightness", .binding_type = TK_MENU_BINDING_INT, .binding = &(global_datastore.brightness_settings.automatic)},
      (tk_menu_item_t){.type = TK_MENU_ITEM_SLIDER, .desc = "Brightness", .binding_type = TK_MENU_BINDING_DOUBLE, .binding_min = 0, .binding_max = 1, .binding_steps = 32, .binding = &(global_datastore.brightness_settings.level)}};

  static tk_menu_t menu_conf = {
      .items_count = 2,
      .items = menu_items};

  lv_obj_t *menu = tk_menu_create(menu_container, group, &menu_conf);

  // Group
  lv_indev_set_group(encoder_indev, group);

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

  tk_top_bar_configuration_t tb_conf = {
      .title = "Brightness"};

  // Return struct
  tk_view_t main_view = {
      .content = view_content,
      .bottom_bar_configuration = bb_conf,
      .top_bar_configuration = tb_conf};

  ESP_LOGD(TAG, "View built successfully.");

  return main_view;
}
