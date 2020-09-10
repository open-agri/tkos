#include "../views.h"
#include "engine/engine.h"

#include "esp_log.h"

#include <stdio.h>
#include <stdlib.h>

#define TAG "Main view"

// Updatable widgets:
lv_obj_t *arc_l;
lv_obj_t *arc_l_big_label;
lv_obj_t *arc_l_small_label;
lv_obj_t *arc_r;
lv_obj_t *arc_r_big_label;

static void refresh_cb(lv_obj_t *obj, lv_event_t event)
{
  if (event != LV_EVENT_REFRESH)
    return;

  // TODO: Variable arc maximum stored in nvs
  // TODO: Implement kph/mph in nvs
  if (obj == arc_l)
  {
  }
  else if (obj == arc_l_big_label)
  {
  }
  else if (obj == arc_l_small_label)
  {
  }
  else if (obj == arc_r)
  {
    if (tk_engine_last_data.rpm_available)
    {
      lv_arc_set_range(obj, 0, 5000);
      lv_arc_set_value(obj, (int)tk_engine_last_data.rpm);
    }
    else
    {
      lv_arc_set_value(obj, 0);
    }
  }
  else if (obj == arc_r_big_label)
  {
    if (tk_engine_last_data.rpm_available)
    {
      char val[5];
      itoa((int)tk_engine_last_data.rpm, val, 10);
      lv_label_set_text(obj, val);
    }
    else
    {
      lv_label_set_text(obj, "---");
    }
  }
}

static void right_button_event_callback()
{
  view_navigate(build_menu_view, true);
}

tk_view_t build_main_view()
{

  // Content
  lv_obj_t *main_view_content = lv_cont_create(NULL, NULL);
  lv_obj_add_style(main_view_content, LV_CONT_PART_MAIN, &tk_style_far_background);

  // Compass
  // TODO: Compass builder.

  // Arcs
  lv_obj_t *dashboard_container = lv_cont_create(main_view_content, NULL);
  lv_obj_add_style(dashboard_container, LV_CONT_PART_MAIN, &tk_style_no_background_borders);
  lv_cont_set_fit2(dashboard_container, LV_FIT_MAX, LV_FIT_TIGHT);
  lv_cont_set_layout(dashboard_container, LV_LAYOUT_PRETTY_MID);
  lv_obj_set_style_local_pad_hor(dashboard_container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 48);
  lv_obj_set_style_local_pad_ver(dashboard_container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);

  // Left arc
  arc_l = lv_arc_create(dashboard_container, NULL);
  lv_obj_add_style(arc_l, LV_CONT_PART_MAIN, &tk_style_no_background_borders);
  lv_arc_set_angles(arc_l, 60, 190);
  lv_arc_set_bg_angles(arc_l, 60, 300);
  lv_arc_set_rotation(arc_l, 90);
  lv_obj_set_size(arc_l, 160, 160);
  lv_obj_set_style_local_pad_all(arc_l, LV_ARC_PART_BG, LV_STATE_DEFAULT, 0);
  lv_obj_set_event_cb(arc_l, refresh_cb);

  // Container
  lv_obj_t *arc_l_inner_cont = lv_cont_create(arc_l, NULL);
  lv_obj_set_size(arc_l_inner_cont, 90, 90);
  lv_obj_align(arc_l_inner_cont, arc_l, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_style(arc_l_inner_cont, LV_CONT_PART_MAIN, &tk_style_no_background_borders);

  // Text
  arc_l_big_label = lv_label_create(arc_l_inner_cont, NULL);
  lv_obj_set_style_local_text_font(arc_l_big_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_THEME_DEFAULT_FONT_TITLE);
  lv_label_set_text(arc_l_big_label, "---");
  lv_obj_align(arc_l_big_label, arc_l_inner_cont, LV_ALIGN_IN_TOP_MID, 0, 8);
  lv_obj_set_event_cb(arc_l_big_label, refresh_cb);

  arc_l_small_label = lv_label_create(arc_l_inner_cont, NULL);
  lv_obj_set_style_local_text_font(arc_l_small_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_THEME_DEFAULT_FONT_SUBTITLE);
  lv_obj_set_style_local_text_color(arc_l_small_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, TK_COLOR_GREY_DARK);
  lv_label_set_text(arc_l_small_label, "km/h");
  lv_obj_align(arc_l_small_label, arc_l_big_label, LV_ALIGN_OUT_BOTTOM_MID, 0, -8);
  lv_obj_set_event_cb(arc_l_small_label, refresh_cb);

  // Right arc
  arc_r = lv_arc_create(dashboard_container, arc_l);
  lv_arc_set_rotation(arc_r, 90);
  lv_obj_set_size(arc_r, 160, 160);
  lv_obj_set_event_cb(arc_r, refresh_cb);

  // Container
  lv_obj_t *arc_r_inner_cont = lv_cont_create(arc_r, NULL);
  lv_obj_set_size(arc_r_inner_cont, 90, 90);
  lv_obj_align(arc_r_inner_cont, arc_r, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_style(arc_r_inner_cont, LV_CONT_PART_MAIN, &tk_style_no_background_borders);

  // Text
  arc_r_big_label = lv_label_create(arc_r_inner_cont, NULL);
  lv_obj_set_style_local_text_font(arc_r_big_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_THEME_DEFAULT_FONT_TITLE);
  lv_label_set_text(arc_r_big_label, "---");
  lv_obj_align(arc_r_big_label, arc_r_inner_cont, LV_ALIGN_IN_TOP_MID, 0, 8);
  lv_obj_set_event_cb(arc_r_big_label, refresh_cb);

  lv_obj_t *arc_r_small_label = lv_label_create(arc_r_inner_cont, NULL);
  lv_obj_set_style_local_text_font(arc_r_small_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_THEME_DEFAULT_FONT_SUBTITLE);
  lv_obj_set_style_local_text_color(arc_r_small_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, TK_COLOR_GREY_DARK);
  lv_label_set_text(arc_r_small_label, "rpm");
  lv_obj_align(arc_r_small_label, arc_r_big_label, LV_ALIGN_OUT_BOTTOM_MID, 0, -8);

  lv_obj_align(dashboard_container, main_view_content, LV_ALIGN_CENTER, 0, 32);

  // Bottom bar configuration
  tk_bottom_bar_button_t right = {
      .text = "Menu",
      .click_callback = right_button_event_callback};

  tk_bottom_bar_button_t left = {
      .text = "Brightness",
      .click_callback = NULL};

  tk_bottom_bar_configuration_t bb_conf = {
      .right_button = right,
      .left_button = left};

  // Return struct
  tk_view_t main_view = {
      .content = main_view_content,
      .bottom_bar_configuration = bb_conf};

  return main_view;
}
