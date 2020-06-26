#include "lvgl/lvgl.h"
#include "bars.h"
#include "views.h"

tk_void_callback left_button_click_cb;
tk_void_callback right_button_click_cb;

bool left_menu_open = false;
bool right_menu_open = false;

static void left_button_event_callback(lv_obj_t *obj, lv_event_t event)
{

    switch (event)
    {
    case LV_EVENT_CLICKED:
        view_navigate_back();
        break;
    }
}

lv_obj_t *build_bottom_bar(tk_bottom_bar_configuration *configuration)
{
    /* CALLBACK SETUP */
    

    /* BACKGROUND */
    lv_obj_t *bottom_bar = lv_cont_create(lv_layer_top(), NULL);

    lv_obj_set_height(bottom_bar, 36);
    lv_obj_set_width(bottom_bar, 480);

    lv_obj_set_style_local_bg_color(bottom_bar, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_radius(bottom_bar, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_border_width(bottom_bar, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);

    /* LEFT BUTTON */
    lv_obj_t *left_button = lv_btn_create(bottom_bar, NULL);
    lv_obj_set_height(left_button, 36);
    lv_obj_set_event_cb(left_button, left_button_event_callback);

    lv_obj_set_style_local_bg_color(left_button, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_bg_color(left_button, LV_BTN_PART_MAIN, LV_STATE_PRESSED, LV_THEME_DEFAULT_COLOR_PRIMARY);
    lv_obj_set_style_local_radius(left_button, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_border_width(left_button, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_outline_width(left_button, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);

    lv_obj_t *label = lv_label_create(left_button, NULL);
    lv_label_set_text(label, "Back");

    lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);

    /* RIGHT_BUTTON */
    lv_obj_t *right_button = lv_btn_create(bottom_bar, left_button);
    lv_obj_align(right_button, bottom_bar, LV_ALIGN_IN_RIGHT_MID, 0, 0);
    label = lv_label_create(right_button, label);
    lv_label_set_text(label, "Select");

    return bottom_bar;
}