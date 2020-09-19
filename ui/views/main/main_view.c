/**
 * @file main_view.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief The main view builder.
 * @version 0.1
 * @date 2020-09-10
 *
 *
 */

#include "model/datastore.h"
#include "ui/views.h"

#include "esp_log.h"

#include <stdio.h>
#include <stdlib.h>

#define TAG "Main view"

// Updatable widgets:
static lv_obj_t *arc_l;
static lv_obj_t *arc_l_big_label;
static lv_obj_t *arc_l_small_label;
static lv_obj_t *arc_r;
static lv_obj_t *arc_r_big_label;

// TODO: Move to array.
// TODO: Check defaults coherency in callback and in generator code.

/**
 * @brief Pushes new data to the widgets when they receive a refresh event.
 *
 * @param obj The widget that called this callback function.
 * @param event The event that the widget received.
 */
static void refresh_cb(lv_obj_t *obj, lv_event_t event) {
  if (event != LV_EVENT_REFRESH)
    return;

  // TODO: Variable arc maximum stored in nvs
  // TODO: Implement kph/mph in nvs

  // Left arc
  if (obj == arc_l) {
    ESP_LOGD(
        TAG,
        "Received a refresh event for left arc, value is <TODO> [km/h - MPH].");
    lv_arc_set_value(obj, 0);
  }
  // Left arc's value label
  else if (obj == arc_l_big_label) {
    ESP_LOGD(TAG, "Received a refresh event for left value label, value is "
                  "<TODO> [km/h - MPH].");
  }
  // Left arc's unit label
  else if (obj == arc_l_small_label) {
    ESP_LOGD(
        TAG,
        "Received a refresh event for left unit label, unit is [km/h - MPH].");
  }
  // Right arc
  else if (obj == arc_r) {
    if (global_datastore.engine_data.rpm_available) {
      ESP_LOGD(TAG,
               "Received a refresh event for right arc, value is %.2f RPM.",
               global_datastore.engine_data.rpm);
      lv_arc_set_value(obj, (int)global_datastore.engine_data.rpm);
    } else {
      ESP_LOGW(TAG, "Received a refresh event for right arc, but RPM data is "
                    "not available.");
      lv_arc_set_value(obj, 0);
    }
  }
  // Right arc's value label
  else if (obj == arc_r_big_label) {
    if (global_datastore.engine_data.rpm_available) {
      char val[5];
      itoa((int)global_datastore.engine_data.rpm, val, 10);
      lv_label_set_text(obj, val);
      ESP_LOGD(TAG,
               "Received a refresh event for right arc label, content is %s.",
               val);
    } else {
      ESP_LOGW(TAG, "Received a refresh event for right value label, but RPM "
                    "data is not available.");
      lv_label_set_text(obj, "---");
    }
  }
}

/**
 * @brief The bottom bar's right button click callback.
 *
 */
static void right_button_click_callback() {
  ESP_LOGI(TAG, "Right button clicked. Navigating to menu view.");
  view_navigate(build_menu_view, true);
}

/**
 * @brief The bottom bar's left button click callback.
 *
 */
static void left_button_click_callback() {
  ESP_LOGI(TAG, "Left button clicked. Navigating to brightness view.");
  view_navigate(build_brightness_view, true);
}

/**
 * @brief The main view generator.
 *
 * @return tk_view_t The generated view.
 */
tk_view_t build_main_view() {

  ESP_LOGI(TAG, "Building view.");

  // Content
  lv_obj_t *view_content = lv_cont_create(NULL, NULL);
  lv_obj_add_style(view_content, LV_CONT_PART_MAIN, &tk_style_far_background);

  // Compass
  // TODO: Compass builder.

  // Arcs
  lv_obj_t *dashboard_container = lv_cont_create(view_content, NULL);
  lv_obj_add_style(dashboard_container, LV_CONT_PART_MAIN,
                   &tk_style_no_background_borders);
  lv_cont_set_fit2(dashboard_container, LV_FIT_MAX, LV_FIT_TIGHT);
  lv_cont_set_layout(dashboard_container, LV_LAYOUT_PRETTY_MID);
  lv_obj_set_style_local_pad_hor(dashboard_container, LV_CONT_PART_MAIN,
                                 LV_STATE_DEFAULT, 48);
  lv_obj_set_style_local_pad_ver(dashboard_container, LV_CONT_PART_MAIN,
                                 LV_STATE_DEFAULT, 0);

  // Left arc
  // TODO: Eh?
  // !!!!!!! WHAT? ANGLES? !!!!!!!!!!
  arc_l = lv_arc_create(dashboard_container, NULL);
  lv_obj_add_style(arc_l, LV_CONT_PART_MAIN, &tk_style_no_background_borders);
  lv_arc_set_bg_angles(arc_l, 60, 300);
  lv_arc_set_angles(arc_l, 60, 300);
  lv_arc_set_range(arc_l, 0, 1000); // km/h * 10
  lv_arc_set_rotation(arc_l, 90);
  lv_arc_set_value(arc_l, 0);
  lv_obj_set_size(arc_l, 160, 160);
  lv_obj_set_style_local_pad_all(arc_l, LV_ARC_PART_BG, LV_STATE_DEFAULT, 0);
  lv_obj_set_event_cb(arc_l, refresh_cb);

  // Container
  lv_obj_t *arc_l_inner_cont = lv_cont_create(arc_l, NULL);
  lv_obj_set_size(arc_l_inner_cont, 90, 90);
  lv_obj_align(arc_l_inner_cont, arc_l, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_style(arc_l_inner_cont, LV_CONT_PART_MAIN,
                   &tk_style_no_background_borders);

  // Text
  arc_l_big_label = lv_label_create(arc_l_inner_cont, NULL);
  lv_obj_set_style_local_text_font(arc_l_big_label, LV_LABEL_PART_MAIN,
                                   LV_STATE_DEFAULT,
                                   LV_THEME_DEFAULT_FONT_TITLE);
  lv_label_set_text(arc_l_big_label, "---");
  lv_obj_align(arc_l_big_label, arc_l_inner_cont, LV_ALIGN_IN_TOP_MID, 0, 8);
  lv_obj_set_event_cb(arc_l_big_label, refresh_cb);

  arc_l_small_label = lv_label_create(arc_l_inner_cont, NULL);
  lv_obj_set_style_local_text_font(arc_l_small_label, LV_LABEL_PART_MAIN,
                                   LV_STATE_DEFAULT,
                                   LV_THEME_DEFAULT_FONT_SUBTITLE);
  lv_obj_set_style_local_text_color(arc_l_small_label, LV_LABEL_PART_MAIN,
                                    LV_STATE_DEFAULT, TK_COLOR_GREY_DARK);
  lv_label_set_text(arc_l_small_label, "km/h");
  lv_obj_align(arc_l_small_label, arc_l_big_label, LV_ALIGN_OUT_BOTTOM_MID, 0,
               -8);
  lv_obj_set_event_cb(arc_l_small_label, refresh_cb);

  // Right arc
  arc_r = lv_arc_create(dashboard_container, NULL);
  lv_obj_add_style(arc_r, LV_CONT_PART_MAIN, &tk_style_no_background_borders);
  lv_arc_set_bg_angles(arc_r, 60, 300);
  lv_arc_set_angles(arc_r, 60, 300);
  lv_arc_set_range(arc_r, 0, 2500);
  lv_arc_set_rotation(arc_r, 90);
  lv_arc_set_value(arc_r, 0);
  lv_obj_set_size(arc_r, 160, 160);
  lv_obj_set_style_local_pad_all(arc_r, LV_ARC_PART_BG, LV_STATE_DEFAULT, 0);
  lv_obj_set_event_cb(arc_r, refresh_cb);

  // Container
  lv_obj_t *arc_r_inner_cont = lv_cont_create(arc_r, NULL);
  lv_obj_set_size(arc_r_inner_cont, 90, 90);
  lv_obj_align(arc_r_inner_cont, arc_r, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_style(arc_r_inner_cont, LV_CONT_PART_MAIN,
                   &tk_style_no_background_borders);

  // Text
  arc_r_big_label = lv_label_create(arc_r_inner_cont, NULL);
  lv_obj_set_style_local_text_font(arc_r_big_label, LV_LABEL_PART_MAIN,
                                   LV_STATE_DEFAULT,
                                   LV_THEME_DEFAULT_FONT_TITLE);
  lv_label_set_text(arc_r_big_label, "---");
  lv_obj_align(arc_r_big_label, arc_r_inner_cont, LV_ALIGN_IN_TOP_MID, 0, 8);
  lv_obj_set_event_cb(arc_r_big_label, refresh_cb);

  lv_obj_t *arc_r_small_label = lv_label_create(arc_r_inner_cont, NULL);
  lv_obj_set_style_local_text_font(arc_r_small_label, LV_LABEL_PART_MAIN,
                                   LV_STATE_DEFAULT,
                                   LV_THEME_DEFAULT_FONT_SUBTITLE);
  lv_obj_set_style_local_text_color(arc_r_small_label, LV_LABEL_PART_MAIN,
                                    LV_STATE_DEFAULT, TK_COLOR_GREY_DARK);
  lv_label_set_text(arc_r_small_label, "rpm");
  lv_obj_align(arc_r_small_label, arc_r_big_label, LV_ALIGN_OUT_BOTTOM_MID, 0,
               -8);

  lv_obj_align(dashboard_container, view_content, LV_ALIGN_CENTER, 0, 32);

  // Bottom bar configuration
  tk_bottom_bar_button_t right_bar_button = {
      .text = "Menu", .click_callback = right_button_click_callback};

  tk_bottom_bar_button_t left_bar_button = {.text = "Brightness",
                                 .click_callback = left_button_click_callback};

  tk_bottom_bar_configuration_t bb_conf = {.right_button = right_bar_button,
                                           .left_button = left_bar_button};

  // Return struct
  tk_view_t main_view = {.content = view_content,
                         .bottom_bar_configuration = bb_conf};

  ESP_LOGD(TAG, "View built successfully.");

  return main_view;
}
