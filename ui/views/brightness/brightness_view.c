/**
 * @file brightness_view.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief The brightness view builder.
 * @version 0.1
 * @date 2020-09-10
 *
 *
 */

#include "esp_log.h"
#include "model/datastore.h"
#include "model/nvsettings.h"
#include "ui/menu/menu.h"
#include "ui/views.h"

#include <stdio.h>

#define TAG "Brightness view"

static lv_group_t *group;
static tk_menu_item_t menu_current_item;
static char lb_string[30];
static char rb_string[30];

static tk_bottom_bar_button_t right_bar_button;
static tk_bottom_bar_button_t left_bar_button;
static tk_bottom_bar_configuration_t bb_conf;
static tk_top_bar_configuration_t tb_conf;

// Callback declarations
TK_MENU_VALUE_CHANGE_CB_DECLARE(auto_brightness_cb);
TK_MENU_VALUE_CHANGE_CB_DECLARE(brightness_level_cb);
static void right_button_update(tk_menu_item_t *focused);

// Menu
static tk_menu_item_t auto_brightness_switch = {
    .type = TK_MENU_ITEM_SWITCH,
    .desc = "Automatic brightness",
    .button_string = "Toggle   " LV_SYMBOL_EDIT,
    .editing_button_string = "Toggle   " LV_SYMBOL_EDIT,
    .binding_type = TK_MENU_BINDING_INT,
    .binding = &(global_datastore.brightness_settings.automatic),
    .value_change_cb = auto_brightness_cb};

static tk_menu_item_t brightness_slider = {
    .type = TK_MENU_ITEM_SLIDER,
    .desc = "Brightness",
    .button_string = "Edit   " LV_SYMBOL_EDIT,
    .editing_button_string = "Done   " LV_SYMBOL_OK,
    .binding_type = TK_MENU_BINDING_DOUBLE,
    .binding_min = 0,
    .binding_max = 1,
    .binding_steps = 400,
    .binding = &(global_datastore.brightness_settings.level),
    .value_change_cb = brightness_level_cb};

static tk_menu_t menu_conf = {
    .items_count = 2,
    .items = {&auto_brightness_switch, &brightness_slider},
    .focus_change_cb = right_button_update};

// Callbacks
TK_MENU_VALUE_CHANGE_CB_DECLARE(brightness_level_cb) {
  double val = *(double *)sender->binding;
  ESP_LOGI(TAG, "Level setting changed to %.2f.", val);

  nv_set_brightness_man_level(val);
}

TK_MENU_VALUE_CHANGE_CB_DECLARE(auto_brightness_cb) {
  bool val = *(bool *)sender->binding;
  ESP_LOGI(TAG, "Automatic setting changed to %d.", val);
  brightness_slider.disabled = val;
  brightness_slider.binding_steps = val ? 400 : 16;

  nv_set_brightness_auto(val);
}

static void right_button_update(tk_menu_item_t *focused) {
  // Update current item if passed
  if (focused != NULL)
    menu_current_item = *focused;

  // Otherwise toggle editing
  if (lv_group_get_editing(group)) {
    strcpy(rb_string, menu_current_item.editing_button_string);
  } else {
    strcpy(rb_string, menu_current_item.button_string);
  }

  lv_event_send_refresh_recursive(lv_scr_act());
}

/**
 * @brief The bottom bar's left button click callback.
 *
 */
static void left_button_click_callback() {
  ESP_LOGI(TAG, "Left button pressed.");
  view_navigate_back();
}

/**
 * @brief The bottom bar's right button click callback.
 *
 */
static void right_button_click_callback() {
  ESP_LOGI(TAG, "Right button pressed.");
  // Toggle editing
  lv_group_set_editing(group, !lv_group_get_editing(group));
  right_button_update(NULL);
}

/**
 * @brief The brightness view generator.
 *
 * @return tk_view_t The generated view.
 */
tk_view_t build_brightness_view() {

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
  lv_obj_t *menu = tk_menu_create(menu_container, group, &menu_conf);

  // Group
  lv_indev_set_group(encoder_indev, group);

  // Prevent crash when current current item was never set
  menu_current_item = *tk_menu_get_current_item(group);

  // Bottom bar configuration
  right_bar_button = (tk_bottom_bar_button_t){
      .text = rb_string, .click_callback = right_button_click_callback};
  right_bar_button.disabled = &(menu_current_item.disabled); // Yeah

  left_bar_button = (tk_bottom_bar_button_t){
      .text = lb_string, .click_callback = left_button_click_callback};

  static tk_bottom_bar_configuration_t bb_conf;
  bb_conf.right_button = right_bar_button;
  bb_conf.left_button = left_bar_button;

  tb_conf.title = "Brightness";

  // Return struct
  tk_view_t main_view = {.content = view_content,
                         .bottom_bar_configuration = bb_conf,
                         .top_bar_configuration = tb_conf};

  ESP_LOGD(TAG, "View built successfully.");

  return main_view;
}
