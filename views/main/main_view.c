#include "../views.h"

#include <stdio.h>

// TODO: Create refresher function

static void button_event_callback()
{
  // case LV_EVENT_CLICKED:view_navigate(build_settings_info_view, true);
}

void first_callback() { printf("First pressed.\n"); }
void second_callback() { printf("Second pressed.\n"); }
void third_callback() { printf("Third pressed.\n"); }
void fourth_callback() { printf("Fourth pressed.\n"); }

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
  lv_obj_add_style(dashboard_container, LV_CONT_PART_MAIN, &tk_style_no_background_borders);
  lv_cont_set_fit2(dashboard_container, LV_FIT_MAX, LV_FIT_TIGHT);
  lv_cont_set_layout(dashboard_container, LV_LAYOUT_PRETTY_MID);
  lv_obj_set_style_local_pad_hor(dashboard_container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 48);
  lv_obj_set_style_local_pad_ver(dashboard_container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);

  // Left arc
  lv_obj_t *arc_l = lv_arc_create(dashboard_container, NULL);
  lv_obj_add_style(arc_l, LV_CONT_PART_MAIN, &tk_style_no_background_borders);
  lv_arc_set_angles(arc_l, 60, 190);
  lv_arc_set_bg_angles(arc_l, 60, 300);
  lv_arc_set_rotation(arc_l, 90);
  lv_obj_set_size(arc_l, 160, 160);
  lv_obj_set_style_local_pad_all(arc_l, LV_ARC_PART_BG, LV_STATE_DEFAULT, 0);

  // Container
  lv_obj_t *arc_l_inner_cont = lv_cont_create(arc_l, NULL);
  lv_obj_set_size(arc_l_inner_cont, 90, 90);
  lv_obj_align(arc_l_inner_cont, arc_l, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_style(arc_l_inner_cont, LV_CONT_PART_MAIN, &tk_style_no_background_borders);

  // Text
  lv_obj_t *arc_l_big_label = lv_label_create(arc_l_inner_cont, NULL);
  lv_obj_set_style_local_text_font(arc_l_big_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_THEME_DEFAULT_FONT_TITLE);
  lv_label_set_text(arc_l_big_label, "34.2");
  lv_obj_align(arc_l_big_label, arc_l_inner_cont, LV_ALIGN_IN_TOP_MID, 0, 8);

  lv_obj_t *arc_l_small_label = lv_label_create(arc_l_inner_cont, NULL);
  lv_obj_set_style_local_text_font(arc_l_small_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_THEME_DEFAULT_FONT_SUBTITLE);
  lv_obj_set_style_local_text_color(arc_l_small_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, TK_COLOR_GREY_DARK);
  lv_label_set_text(arc_l_small_label, "km/h");
  lv_obj_align(arc_l_small_label, arc_l_big_label, LV_ALIGN_OUT_BOTTOM_MID, 0, -8);

  // Right arc
  lv_obj_t *arc_r = lv_arc_create(dashboard_container, arc_l);
  lv_arc_set_rotation(arc_r, 90);
  lv_obj_set_size(arc_r, 160, 160);

  // Container
  lv_obj_t *arc_r_inner_cont = lv_cont_create(arc_r, NULL);
  lv_obj_set_size(arc_r_inner_cont, 90, 90);
  lv_obj_align(arc_r_inner_cont, arc_r, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_style(arc_r_inner_cont, LV_CONT_PART_MAIN, &tk_style_no_background_borders);

  // Text
  lv_obj_t *arc_r_big_label = lv_label_create(arc_r_inner_cont, NULL);
  lv_obj_set_style_local_text_font(arc_r_big_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_THEME_DEFAULT_FONT_TITLE);
  lv_label_set_text(arc_r_big_label, "1444");
  lv_obj_align(arc_r_big_label, arc_r_inner_cont, LV_ALIGN_IN_TOP_MID, 0, 8);

  lv_obj_t *arc_r_small_label = lv_label_create(arc_r_inner_cont, NULL);
  lv_obj_set_style_local_text_font(arc_r_small_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_THEME_DEFAULT_FONT_SUBTITLE);
  lv_obj_set_style_local_text_color(arc_r_small_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, TK_COLOR_GREY_DARK);
  lv_label_set_text(arc_r_small_label, "rpm");
  lv_obj_align(arc_r_small_label, arc_r_big_label, LV_ALIGN_OUT_BOTTOM_MID, 0, -8);

  lv_obj_align(dashboard_container, main_view_content, LV_ALIGN_CENTER, 0, 32);

  // Bottom bar cofniguration
  tk_bottom_bar_button right = {
      .text = "Menu",
      .click_callback = button_event_callback,
      .menu = {(tk_menu_item){.text = "First", .click_callback = first_callback},
               (tk_menu_item){.text = "Second", .click_callback = second_callback}},
      .items_count = 2};

  tk_bottom_bar_configuration bb_conf = {
      .right_button = right};

  // Return struct
  tk_view main_view = {
      .content = main_view_content,
      .bottom_bar_configuration = bb_conf};

  return main_view;
}
