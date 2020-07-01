#include "bars.h"
#include "lvgl/lvgl.h"
#include "../tkos.h"
#include "styles/tk_style.h"

bool menu_open = false;

// TODO: Hide these vars
tk_bottom_bar_configuration original_bottom_bar_configuration;
bool left_open_flag = false;
bool right_open_flag = false;

lv_obj_t *menu;
lv_obj_t *bottom_bar;
lv_group_t *menu_group;

void hide_menu(lv_obj_t *menu) {

    // Hide the menu
    lv_obj_del(menu);

    // Reset variables and configuration
    menu_open = false;

    // Rebuild the bar with original bb_conf
    lv_obj_del(bottom_bar);
    build_bottom_bar(original_bottom_bar_configuration, false);

    // Note: build_bottom_bar automatically sets bottom_bar pointer
    lv_obj_align(bottom_bar, lv_scr_act(), LV_ALIGN_IN_BOTTOM_MID, 0, -8);

}

void show_menu(tk_bottom_bar_configuration current_bb_conf, bool left) {

    unsigned int items = left ? current_bb_conf.left_button.items_count : current_bb_conf.right_button.items_count;
    tk_menu_item *menu_items = left ? current_bb_conf.left_button.menu : current_bb_conf.right_button.menu;

    // Menu generation
    menu = lv_list_create(lv_scr_act(), NULL);
    lv_list_set_scrollbar_mode(menu, LV_SCROLLBAR_MODE_AUTO);
    lv_list_set_anim_time(menu, 200);
    lv_obj_set_width(menu, 120);
    lv_obj_set_height(menu, items <= 7 ? items * 36 + 24 : 276);
    lv_obj_align(menu, bottom_bar, left ? LV_ALIGN_OUT_TOP_LEFT : LV_ALIGN_OUT_TOP_RIGHT, 0, -8);
    lv_obj_add_style(menu, LV_LIST_PART_BG, &tk_style_menu);

    // Group init (list does not work with this configuration)
    menu_group = lv_group_create();

#ifdef SIMULATOR
    lv_indev_set_group(lv_indev_get_act(), menu_group);
#else
    lv_indev_set_group(encoder_indev, menu_group);
#endif

    lv_obj_t *btn;
    for (int i = 0; i < items; i++) {
        btn = lv_list_add_btn(menu, NULL, menu_items[i].text);
        lv_obj_add_style(btn, LV_BTN_PART_MAIN, &tk_style_menu_button);
        lv_obj_set_size(btn, 130, 36);

        // Set callback to be retrieved later in user_data
        // TODO: Create a callback which executes this pointer on tap, for eventual future touchscreen systems
        lv_obj_set_user_data(btn, menu_items[i].click_callback);

        // Add to group
        lv_group_add_obj(menu_group, btn);
    }

    // Update variables
    menu_open = true;

    // Update bar
    lv_obj_del(bottom_bar);
    tk_bottom_bar_configuration temp_bb_conf = current_bb_conf;
    strcpy(temp_bb_conf.right_button.text, "Select");
    strcpy(temp_bb_conf.left_button.text, "Back");
    bottom_bar = build_bottom_bar(temp_bb_conf, false);
    lv_obj_align(bottom_bar, lv_scr_act(), LV_ALIGN_IN_BOTTOM_MID, 0, -8);
    lv_obj_move_foreground(bottom_bar);
}

static void left_button_event_callback(lv_obj_t *obj, lv_event_t event) {

    switch (event) {
        case LV_EVENT_CLICKED:
            // Clicked: execute callback when menus closed, close menu when open
            if (menu_open) {
                // Close menu
                hide_menu(menu);
            } else {
                if (original_bottom_bar_configuration.left_button.click_callback != NULL)
                    (original_bottom_bar_configuration.left_button.click_callback)();
            }

            break;

        case LV_EVENT_LONG_PRESSED:
            // Long press: show menu when both are closed and a menu is available
            if (!menu_open && original_bottom_bar_configuration.left_button.items_count > 0) {
                // Using a flag in order to delay the appearance of the menu on button release
                left_open_flag = true;
            }
            break;

        case LV_EVENT_RELEASED:
            // No continued pressure
            lv_obj_set_state(obj, LV_STATE_DEFAULT);

            if (left_open_flag) {
                show_menu(original_bottom_bar_configuration, true);
                left_open_flag = false;
            }
            break;
    }
}

static void right_button_event_callback(lv_obj_t *obj, lv_event_t event) {

    switch (event) {
        case LV_EVENT_CLICKED:
            // Clicked: execute callback when menus closed, select item when open
            if (menu_open) {
                // Select item (execute function pointed by user data of the focused button)
                ((tk_void_callback) (lv_group_get_focused(menu_group)->user_data))();
                // Close menu
                hide_menu(menu);
            } else {
                if (original_bottom_bar_configuration.right_button.click_callback != NULL)
                    (original_bottom_bar_configuration.right_button.click_callback)();
            }

            break;

        case LV_EVENT_LONG_PRESSED:
            // Long press: show menu when both are closed and a menu is available
            if (!menu_open && original_bottom_bar_configuration.right_button.items_count > 0) {
                // Using a flag in order to delay the appearance of the menu on button release
                right_open_flag = true;
            }
            break;

        case LV_EVENT_RELEASED:
            // No continued pressure
            lv_obj_set_state(obj, LV_STATE_DEFAULT);

            if (right_open_flag) {
                show_menu(original_bottom_bar_configuration, false);
                right_open_flag = false;
            }
            break;
    }
}

lv_obj_t *build_bottom_bar(tk_bottom_bar_configuration configuration, bool original) {

    /* BACKGROUND */

    /* SAVE IN "GLOBAL" POINTER FOR ALLOWING INTERNAL CHANGES */
    bottom_bar = lv_cont_create(lv_scr_act(), NULL);
    lv_obj_set_height(bottom_bar, 36);
    lv_obj_set_width(bottom_bar, 464);
    lv_obj_add_style(bottom_bar, LV_CONT_PART_MAIN, &tk_style_bb);

    /* LEFT BUTTON */
    if (strlen(configuration.left_button.text)) {
        lv_obj_t *left_button = lv_btn_create(bottom_bar, NULL);
        lv_obj_set_size(left_button, 120, 36);
        lv_obj_set_event_cb(left_button, left_button_event_callback);
        lv_obj_add_style(left_button, LV_BTN_PART_MAIN, &tk_style_menu_button);

        lv_obj_t *label = lv_label_create(left_button, NULL);
        lv_label_set_text(label, configuration.left_button.text);
    }

    /* RIGHT BUTTON */
    if (strlen(configuration.right_button.text)) {
        lv_obj_t *right_button = lv_btn_create(bottom_bar, NULL);
        lv_obj_set_size(right_button, 120, 36);
        lv_obj_set_event_cb(right_button, right_button_event_callback);
        lv_obj_add_style(right_button, LV_BTN_PART_MAIN, &tk_style_menu_button);

        lv_obj_align(right_button, bottom_bar, LV_ALIGN_IN_RIGHT_MID, 0, 0);

        lv_obj_t *label = lv_label_create(right_button, NULL);
        lv_label_set_text(label, configuration.right_button.text);
    }


    /* SAVE CONF AND POINTER */
    if (original)
        original_bottom_bar_configuration = configuration;

    return bottom_bar;
}