#include "../views.h"

#include <stdio.h>

static void button_event_callback()
{
  // case LV_EVENT_CLICKED:view_navigate(build_settings_info_view, true);
}

void first_callback() { printf("First pressed."); }
void second_callback() { printf("Second pressed."); }
void third_callback() { printf("Third pressed."); }
void fourth_callback() { printf("Fourth pressed."); }

tk_view build_main_view()
{

  // Content
  lv_obj_t *main_view_content = lv_cont_create(NULL, NULL);
  lv_obj_add_style(main_view_content, LV_CONT_PART_MAIN, &tk_style_far_background);

  // Compass
  // TODO: Compass builder.

  // Arcs
  // TODO: Create style that removes background and border.
  lv_obj_t *dashboard_container = lv_cont_create(main_view_content, NULL);
  lv_cont_set_fit2(dashboard_container, LV_FIT_MAX, LV_FIT_TIGHT);
  lv_cont_set_layout(dashboard_container, LV_LAYOUT_PRETTY_MID);
  lv_obj_set_style_local_pad_hor(dashboard_container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 48);
  lv_obj_set_style_local_pad_ver(dashboard_container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_bg_opa(dashboard_container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_0);
  lv_obj_set_style_local_border_width(dashboard_container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);

  lv_obj_t *arc_l = lv_arc_create(dashboard_container, NULL);
  lv_obj_set_style_local_bg_opa(arc_l, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_0);
  lv_obj_set_style_local_border_width(arc_l, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_arc_set_angles(arc_l, 60, 190);
  lv_arc_set_bg_angles(arc_l, 60, 300);
  lv_arc_set_rotation(arc_l, 90);
  lv_obj_set_size(arc_l, 160, 160);
  lv_obj_set_style_local_pad_all(arc_l, LV_ARC_PART_BG, LV_STATE_DEFAULT, 0);

  lv_obj_t *arc_r = lv_arc_create(dashboard_container, arc_l);
  lv_arc_set_rotation(arc_r, 90);
  lv_obj_set_size(arc_r, 160, 160);

  lv_obj_align(dashboard_container, main_view_content, LV_ALIGN_CENTER, 0, 0);

  // Bottom bar
  tk_bottom_bar_button right = {
      .text = "Menu",
      .click_callback = button_event_callback,
      .items_count = 4,
      .menu = {
          {.text = "First", .click_callback = &first_callback},
          {.text = "Second", .click_callback = &second_callback},
          {.text = "Third", .click_callback = &third_callback},
          {.text = "Fourth", .click_callback = &fourth_callback}

      }};

  tk_bottom_bar_configuration bb_conf = {
      .right_button = right};

  // Return struct
  tk_view main_view = {
      .content = main_view_content,
      .bottom_bar_configuration = bb_conf};

  return main_view;
}
