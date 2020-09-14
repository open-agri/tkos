/**
 * @file menu_view.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief The menu view builder.
 * @version 0.1
 * @date 2020-09-10
 * 
 * 
 */

#include "ui/views.h"
#include "esp_log.h"

#include <stdio.h>

#define TAG "Menu view"

/**
 * @brief The bottom bar's left button click callback.
 * 
 */
static void left_button_click_callback()
{
  ESP_LOGI(TAG, "Left button pressed. Navigating back.");
  view_navigate_back();
}

/**
 * @brief The menu view generator.
 * 
 * @return tk_view_t The generated view.
 */
tk_view_t build_menu_view()
{

  ESP_LOGI(TAG, "Building view.");

  // Content
  lv_obj_t *view_content = lv_cont_create(NULL, NULL);
  lv_obj_add_style(view_content, LV_CONT_PART_MAIN, &tk_style_far_background);

  // Group (for encoder)
  lv_group_t *menu_group = lv_group_create();

  // List in group
  lv_obj_t *list = lv_list_create(view_content, NULL);
  lv_group_add_obj(menu_group, list);
  lv_indev_set_group(encoder_indev, menu_group);
  lv_group_set_editing(menu_group, true);

  lv_obj_add_style(list, LV_LIST_PART_BG, &tk_style_menu_fullscreen);
  lv_obj_add_style(list, LV_LIST_PART_SCROLLABLE, &tk_style_menu_fullscreen);
  lv_obj_set_size(list, 480, 320 - (2 * 36));
  lv_obj_align(list, view_content, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_style(list, LV_CONT_PART_MAIN, &tk_style_far_background);

  for (int i = 0; i < 30; i++)
  {
    lv_obj_t *btn = lv_list_add_btn(list, NULL, "Button");
    lv_obj_add_style(btn, LV_BTN_PART_MAIN, &tk_style_menu_button);
  }

  // Bottom bar configuration
  tk_bottom_bar_button_t right = {
      .text = "Menu",
      .click_callback = NULL,
      .menu = {(tk_menu_item_t){.text = "First", .click_callback = NULL},
               (tk_menu_item_t){.text = "Second", .click_callback = NULL}},
      .items_count = 2};

  tk_bottom_bar_button_t left = {
      .text = LV_SYMBOL_LEFT "   Back",
      .click_callback = left_button_click_callback};

  tk_bottom_bar_configuration_t bb_conf = {
      .right_button = right,
      .left_button = left};

  tk_top_bar_configuration_t tb_conf = {
      .title = "Menu"};

  // Return struct
  tk_view_t main_view = {
      .content = view_content,
      .bottom_bar_configuration = bb_conf,
      .top_bar_configuration = tb_conf};

  ESP_LOGD(TAG, "View built successfully.");

  return main_view;
}
