#include "bars.h"
#include "lvgl/lvgl.h"
#include "../tkos.h"
#include "styles/tk_style.h"

tk_bottom_bar_configuration bb_conf;

int current_menu_index;

bool left_menu_open = false;
bool right_menu_open = false;

lv_obj_t *menu;
lv_obj_t *bottom_bar;
lv_group_t *menu_group;

void show_menu(tk_menu_item *menu_items, int items, bool left) {

  // Menu generation
  menu = lv_list_create(lv_layer_top(), NULL);
  lv_obj_set_width(menu, 80);
  lv_obj_set_height(menu, items * 36);
  lv_obj_align(menu, lv_layer_top(), left ? LV_ALIGN_CENTER : LV_ALIGN_CENTER, 0, 0);


  // Group init
  menu_group = lv_group_create();

#ifdef SIMULATOR
  lv_indev_set_group(lv_indev_get_act(), menu_group);
#elif
  lv_indev_set_group(encoder_indev, menu_group);
#endif

  for(int i = 0; i < items; i++) {
    lv_obj_t *item = lv_btn_create(menu, NULL);
    lv_obj_set_width(item, 80);
    lv_obj_add_style(item, LV_BTN_PART_MAIN, &tk_style_menu_button);

    lv_obj_t *label = lv_label_create(item, NULL);
    lv_label_set_text(label, menu_items[i].text);

    // Add to group
    lv_group_add_obj(menu_group, item);
  }

  // TODO: Update bar

}

static void left_button_event_callback(lv_obj_t *obj, lv_event_t event) {

  switch (event) {
	case LV_EVENT_CLICKED:
	  // Clicked: execute callback when menus closed, close menu when open
	  if (!left_menu_open && !right_menu_open) {
		// TODO: Close menu
	  } else {
	    (bb_conf.left_button.click_callback)();
	  }

	  break;

	case LV_EVENT_LONG_PRESSED:
	  // Long press: show menu when both are closed and a menu is available
	  if (!left_menu_open && !right_menu_open) {
		// TODO: Show left menu
	  }

	  break;
  }
}

static void right_button_event_callback(lv_obj_t *obj, lv_event_t event) {

  switch (event) {
	case LV_EVENT_CLICKED:
	  // Clicked: execute callback when menus closed, select item when open
	  if (!left_menu_open && !right_menu_open) {
		// TODO: Select item
	  } else {
	    (bb_conf.right_button.click_callback)();
	  }

	  break;
	case LV_EVENT_LONG_PRESSED:
	  // Long press: show menu when both are closed and a menu is available
	  if (!left_menu_open && !right_menu_open) {
		show_menu(bb_conf.right_button.menu, bb_conf.right_button.items_count, false);
	  }

	  break;
  }
}

lv_obj_t *build_bottom_bar(tk_bottom_bar_configuration configuration) {
  /* CALLBACK SETUP */
  bb_conf = configuration;

  /* BACKGROUND */
  bottom_bar = lv_cont_create(lv_layer_top(), NULL);

  lv_obj_set_height(bottom_bar, 36);
  lv_obj_set_width(bottom_bar, 480);

  lv_obj_set_style_local_bg_color(bottom_bar, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_set_style_local_radius(bottom_bar, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_border_width(bottom_bar, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);

  /* LEFT BUTTON */
  if (strlen(configuration.left_button.text)) {
	lv_obj_t *left_button = lv_btn_create(bottom_bar, NULL);
	lv_obj_set_height(left_button, 36);
	lv_obj_set_event_cb(left_button, left_button_event_callback);
	lv_obj_add_style(left_button, LV_BTN_PART_MAIN, &tk_style_menu_button);

	lv_obj_t *label = lv_label_create(left_button, NULL);
	lv_label_set_text(label, configuration.left_button.text);

	lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  }

  /* RIGHT BUTTON */
  if (strlen(configuration.right_button.text)) {
	lv_obj_t *right_button = lv_btn_create(bottom_bar, NULL);
	lv_obj_set_height(right_button, 36);
	// TODO: Add delegate
	lv_obj_set_event_cb(right_button, right_button_event_callback);
    lv_obj_add_style(right_button, LV_BTN_PART_MAIN, &tk_style_menu_button);

    lv_obj_align(right_button, bottom_bar, LV_ALIGN_IN_RIGHT_MID, 0, 0);

	lv_obj_t *label = lv_label_create(right_button, NULL);
	lv_label_set_text(label, configuration.right_button.text);

	lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  }

  return bottom_bar;
}