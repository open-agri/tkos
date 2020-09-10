#include "../views.h"

#include <stdio.h>

void left_button_event_callback()
{
  view_navigate_back();
}

tk_view_t build_menu_view()
{

  // Content
  lv_obj_t *main_view_content = lv_cont_create(NULL, NULL);
  lv_obj_add_style(main_view_content, LV_CONT_PART_MAIN, &tk_style_far_background);

  // Group (for encoder)
  lv_group_t *menu_group = lv_group_create();

  // List in group
  lv_obj_t *list = lv_list_create(main_view_content, NULL);
  lv_group_add_obj(menu_group, list);
  lv_indev_set_group(encoder_indev, menu_group);
  lv_group_set_editing(menu_group, true);

  lv_obj_add_style(list, LV_LIST_PART_BG, &tk_style_menu_fullscreen);
  lv_obj_add_style(list, LV_LIST_PART_SCROLLABLE, &tk_style_menu_fullscreen);
  lv_obj_set_size(list, 480, 320 - (2 * 36));
  lv_obj_align(list, main_view_content, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_style(list, LV_CONT_PART_MAIN, &tk_style_far_background);

  for (int i = 0; i < 30; i++)
  {
    lv_obj_t *btn = lv_list_add_btn(list, NULL, "Button");
    lv_obj_add_style(btn, LV_BTN_PART_MAIN, &tk_style_menu_button);
  }

  // Bottom bar cofniguration
  // tk_bottom_bar_button_t right = {
  //     .text = "Menu",
  //     .click_callback = right_button_event_callback,
  //     .menu = {(tk_menu_item_t){.text = "First", .click_callback = first_callback},
  //              (tk_menu_item_t){.text = "Second", .click_callback = second_callback}},
  //     .items_count = 2};

  tk_bottom_bar_button_t left = {
      .text = LV_SYMBOL_LEFT "   Back",
      .click_callback = left_button_event_callback};

  tk_bottom_bar_configuration_t bb_conf = {
      // .right_button = right,
      .left_button = left};

  // Return struct
  tk_view_t main_view = {
      .content = main_view_content,
      .bottom_bar_configuration = bb_conf};

  return main_view;
}
