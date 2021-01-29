/**
 * @file driveshaft_view.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Driveshaft view.
 * @version 0.1
 * @date 2021-01-28
 *
 *
 */

#include "esp_log.h"
#include "ui/views.h"

#include <stdio.h>

#define TAG "Driveshaft view"

/**
 * @brief Pushes new data to the widgets when they receive a refresh event.
 *
 * @param obj The widget that called this callback function.
 * @param event The event that the widget received.
 */
static void refresh_cb(lv_obj_t *obj, lv_event_t event) {
  if (event != LV_EVENT_REFRESH)
    return;

  // Left arc
  if (obj == arc_l) {
    if (global_datastore.location_data.speed_available &&
        global_datastore.location_data.speed > 2.5) {
      ESP_LOGV(TAG,
               "Received a refresh event for left arc, value is %.2f km/h.",
               global_datastore.location_data.speed);
      lv_arc_set_value(obj, (int)(global_datastore.location_data.speed * 10));
    } else {
      lv_arc_set_value(obj, 0);
    }
  }
  // Left arc's value label
  else if (obj == arc_l_big_label) {
    if (global_datastore.location_data.speed_available &&
        global_datastore.location_data.speed > 2.5) {
      char val[10];
      snprintf(val, 10, "%.1f", global_datastore.location_data.speed);
      lv_label_set_text(obj, val);

      ESP_LOGV(TAG,
               "Received a refresh event for right arc label, content is %s.",
               val);
    } else {
      lv_label_set_text(obj, "---");
    }
    lv_obj_realign(obj);
  }
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
}

/**
 * @brief The template view generator.
 *
 * @return tk_view_t The generated view.
 */
tk_view_t build_driveshaft_view() {

  ESP_LOGI(TAG, "Building view.");

  // Content
  lv_obj_t *view_content = lv_cont_create(NULL, NULL);
  lv_obj_add_style(view_content, LV_CONT_PART_MAIN, &tk_style_far_background);

  // Arcs
  lv_obj_t *dashboard_container = lv_cont_create(view_content, NULL);
  lv_obj_add_style(dashboard_container, LV_CONT_PART_MAIN,
                   &tk_style_no_background_borders);
  lv_cont_set_fit2(dashboard_container, LV_FIT_MAX, LV_FIT_TIGHT);
  lv_cont_set_layout(dashboard_container, LV_LAYOUT_PRETTY_MID);
  lv_obj_set_style_local_pad_hor(dashboard_container, LV_CONT_PART_MAIN,
                                 LV_STATE_DEFAULT, 25);
  lv_obj_set_style_local_pad_ver(dashboard_container, LV_CONT_PART_MAIN,
                                 LV_STATE_DEFAULT, 0);

  // Left arc
  arc_l = lv_arc_create(dashboard_container, NULL);
  lv_obj_add_style(arc_l, LV_CONT_PART_MAIN, &tk_style_no_background_borders);
  lv_arc_set_bg_angles(arc_l, 60, 300);
  lv_arc_set_angles(arc_l, 60, 300);
  lv_arc_set_range(arc_l, 0, 2500);
  lv_arc_set_rotation(arc_l, 90);
  lv_arc_set_value(arc_l, 0);
  lv_arc_set_adjustable(arc_l, false);
  lv_obj_set_size(arc_l, 200, 200);
  lv_obj_set_style_local_pad_all(arc_l, LV_ARC_PART_BG, LV_STATE_DEFAULT, 0);
  lv_obj_set_event_cb(arc_l, refresh_cb);

  // Container
  lv_obj_t *arc_l_inner_cont = lv_cont_create(arc_l, NULL);
  lv_obj_set_size(arc_l_inner_cont, 120, 120);
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

  lv_obj_t *arc_l_small_label = lv_label_create(arc_l_inner_cont, NULL);
  lv_obj_set_style_local_text_font(arc_l_small_label, LV_LABEL_PART_MAIN,
                                   LV_STATE_DEFAULT,
                                   LV_THEME_DEFAULT_FONT_SUBTITLE);
  lv_obj_set_style_local_text_color(arc_l_small_label, LV_LABEL_PART_MAIN,
                                    LV_STATE_DEFAULT, TK_COLOR_GREY_DARK);
  lv_label_set_text(arc_l_small_label, "rpm");
  lv_obj_align(arc_l_small_label, arc_l_big_label, LV_ALIGN_OUT_BOTTOM_MID, 0,
               -8);

  lv_obj_align(dashboard_container, view_content, LV_ALIGN_CENTER, 0, 32);

  // Group (for encoder)
  lv_group_t *group = lv_group_create();

  // Bottom bar configuration
  tk_bottom_bar_button_t left_bar_button = {.text = LV_SYMBOL_LEFT "   Back",
                                            .click_callback =
                                                left_button_click_callback};

  tk_bottom_bar_configuration_t bb_conf = {.left_button = left_bar_button};

  tk_top_bar_configuration_t tb_conf = {.title = "Cardano"}

  // Return struct
  tk_view_t main_view = {.content = view_content,
                         .bottom_bar_configuration = bb_conf,
                         .top_bar_configuration = tb_conf};

  ESP_LOGD(TAG, "View built successfully.");

  return main_view;
}
