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

void show_menu(tk_menu_item *menu_items, int items, bool left)
{

	// Menu generation
	menu = lv_list_create(lv_layer_top(), NULL);
	lv_list_set_scrollbar_mode(menu, LV_SCROLLBAR_MODE_AUTO);
	lv_obj_set_width(menu, 120);
	lv_obj_set_height(menu, items <= 7 ? items * 36 + 24 : 276);
	lv_obj_align(menu, bottom_bar, left ? LV_ALIGN_OUT_TOP_LEFT : LV_ALIGN_OUT_TOP_RIGHT, 0, -8);
	lv_obj_add_style(menu, LV_LIST_PART_BG | LV_LIST_PART_SCROLLABLE, &tk_style_menu);

	// Group init (list does not work with this configuration)
	menu_group = lv_group_create();

#ifdef SIMULATOR
	lv_indev_set_group(lv_indev_get_act(), menu_group);
#else
	lv_indev_set_group(encoder_indev, menu_group);
#endif

	lv_obj_t *btn;

	for (int i = 0; i < items; i++)
	{
		btn = lv_list_add_btn(menu, NULL, menu_items[i].text);
		lv_obj_add_style(btn, LV_BTN_PART_MAIN, &tk_style_menu_button);
		lv_obj_set_size(btn, 130, 36);

		// Add to group
		lv_group_add_obj(menu_group, btn);
	}

	lv_list_set_anim_time(menu, 200);

	// TODO: Update bar
}

static void left_button_event_callback(lv_obj_t *obj, lv_event_t event)
{

	switch (event)
	{
	case LV_EVENT_CLICKED:
		// Clicked: execute callback when menus closed, close menu when open
		if (!left_menu_open && !right_menu_open)
		{
			// TODO: Close menu
		}
		else
		{
			(bb_conf.left_button.click_callback)();
		}

		break;

	case LV_EVENT_LONG_PRESSED:
		// Long press: show menu when both are closed and a menu is available
		if (!left_menu_open && !right_menu_open)
		{
			// TODO: Show left menu
		}

		break;
	}
}

static void right_button_event_callback(lv_obj_t *obj, lv_event_t event)
{

	switch (event)
	{
	case LV_EVENT_CLICKED:
		// Clicked: execute callback when menus closed, select item when open
		if (!left_menu_open && !right_menu_open)
		{
			// TODO: Select item
		}
		else
		{
			(bb_conf.right_button.click_callback)();
		}

		break;
	case LV_EVENT_LONG_PRESSED:
		// Long press: show menu when both are closed and a menu is available
		if (!left_menu_open && !right_menu_open)
		{
			show_menu(bb_conf.right_button.menu, bb_conf.right_button.items_count, false);
		}

		break;
	}
}

lv_obj_t *build_bottom_bar(tk_bottom_bar_configuration configuration)
{
	/* CALLBACK SETUP */
	bb_conf = configuration;

	/* BACKGROUND */
	bottom_bar = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_height(bottom_bar, 36);
	lv_obj_set_width(bottom_bar, 464);
	lv_obj_add_style(bottom_bar, LV_CONT_PART_MAIN, &tk_style_bb);

	/* LEFT BUTTON */
	if (strlen(configuration.left_button.text))
	{
		lv_obj_t *left_button = lv_btn_create(bottom_bar, NULL);
		lv_obj_set_size(left_button, 120, 36);
		lv_obj_set_event_cb(left_button, left_button_event_callback);
		lv_obj_add_style(left_button, LV_BTN_PART_MAIN, &tk_style_menu_button);

		lv_obj_t *label = lv_label_create(left_button, NULL);
		lv_label_set_text(label, configuration.left_button.text);
	}

	/* RIGHT BUTTON */
	if (strlen(configuration.right_button.text))
	{
		lv_obj_t *right_button = lv_btn_create(bottom_bar, NULL);
		lv_obj_set_size(right_button, 120, 36);
		lv_obj_set_event_cb(right_button, right_button_event_callback);
		lv_obj_add_style(right_button, LV_BTN_PART_MAIN, &tk_style_menu_button);

		lv_obj_align(right_button, bottom_bar, LV_ALIGN_IN_RIGHT_MID, 0, 0);

		lv_obj_t *label = lv_label_create(right_button, NULL);
		lv_label_set_text(label, configuration.right_button.text);
	}

	return bottom_bar;
}