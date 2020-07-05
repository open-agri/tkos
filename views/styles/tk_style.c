//
// Created by riccardo on 26/06/20.
//

#include "tk_style.h"
#include "malloc.h"

void set_default_shadow(lv_style_t *style, int height) {
    int radius = height * height / 2;
    lv_style_set_shadow_color(style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_shadow_opa(style, LV_STATE_DEFAULT, LV_THEME_DEFAULT_FLAG == LV_THEME_MATERIAL_FLAG_DARK ? LV_OPA_30 : LV_OPA_40);
    lv_style_set_shadow_ofs_y(style, LV_STATE_DEFAULT, 0);
    lv_style_set_shadow_width(style, LV_STATE_DEFAULT, radius);
}

lv_color_t get_themed_bar_background_color() {
    if (LV_THEME_DEFAULT_FLAG == LV_THEME_MATERIAL_FLAG_DARK) {
        return LV_COLOR_BLACK;
    } else {
        return (lv_color_t){225, 225, 225, 255};
    }
}

lv_color_t get_themed_far_background_color() {
    lv_color_t color = get_themed_bar_background_color();
    int mix;
    if (LV_THEME_DEFAULT_FLAG == LV_THEME_MATERIAL_FLAG_DARK) {
        mix = 220;
    } else {
        mix = 0;
    }
    return lv_color_mix(color, LV_COLOR_WHITE, mix);
}

lv_color_t get_themed_near_background_color() {
    lv_color_t color = get_themed_bar_background_color();
    int mix;
    if (LV_THEME_DEFAULT_FLAG == LV_THEME_MATERIAL_FLAG_DARK) {
        mix = 190;
    } else {
        mix = 30;
    }
    return lv_color_mix(color, LV_COLOR_WHITE, mix);
}

// TODO: Add automatic light/dark control from LV_THEME_DEFAULT_FLAG
// TODO: Call on theme change
void tk_styles_init(void) {

    // MENU BUTTON
    lv_style_init(&tk_style_menu_button);
    lv_style_set_bg_opa(&tk_style_menu_button, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_bg_opa(&tk_style_menu_button, LV_STATE_FOCUSED, LV_OPA_TRANSP);
    lv_style_set_bg_opa(&tk_style_menu_button, LV_STATE_PRESSED, LV_OPA_100);
    lv_style_set_bg_opa(&tk_style_menu_button, LV_STATE_EDITED, LV_OPA_100);
    lv_style_set_bg_color(&tk_style_menu_button, LV_STATE_PRESSED, LV_THEME_DEFAULT_COLOR_SECONDARY);
    lv_style_set_bg_color(&tk_style_menu_button, LV_STATE_FOCUSED, LV_THEME_DEFAULT_COLOR_PRIMARY);
    lv_style_set_radius(&tk_style_menu_button, LV_STATE_DEFAULT, 0);
    lv_style_set_border_width(&tk_style_menu_button, LV_STATE_DEFAULT, 0);
    lv_style_set_border_width(&tk_style_menu_button, LV_STATE_FOCUSED, 0);
    lv_style_set_outline_width(&tk_style_menu_button, LV_STATE_DEFAULT, 0);
    lv_style_set_outline_width(&tk_style_menu_button, LV_STATE_FOCUSED, 0);
    lv_style_set_margin_left(&tk_style_menu_button, LV_STATE_DEFAULT, 0);
    lv_style_set_margin_right(&tk_style_menu_button, LV_STATE_DEFAULT, 0);


    // MENU
    lv_style_init(&tk_style_menu);
    set_default_shadow(&tk_style_menu, 7);
    lv_style_set_radius(&tk_style_menu, LV_STATE_DEFAULT, 8);
    lv_style_set_clip_corner(&tk_style_menu, LV_STATE_DEFAULT, true);
    lv_style_set_border_width(&tk_style_menu, LV_STATE_DEFAULT, 0);
    lv_style_set_outline_width(&tk_style_menu, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_left(&tk_style_menu, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_right(&tk_style_menu, LV_STATE_DEFAULT, 0);
    lv_style_set_line_width(&tk_style_menu, LV_STATE_DEFAULT, 0);
    lv_style_set_bg_color(&tk_style_menu, LV_STATE_DEFAULT, get_themed_near_background_color());

    // BACKGROUND
    lv_style_init(&tk_style_far_background);
    lv_color_t aaa = get_themed_far_background_color();
    lv_style_set_bg_color(&tk_style_far_background, LV_STATE_DEFAULT, get_themed_far_background_color());

    // BARS
    lv_style_init(&tk_style_bar);
    set_default_shadow(&tk_style_bar, 9);
    lv_style_set_radius(&tk_style_bar, LV_STATE_DEFAULT, 0);
    lv_style_set_border_width(&tk_style_bar, LV_STATE_DEFAULT, 0);
    lv_style_set_clip_corner(&tk_style_bar, LV_STATE_DEFAULT, true);
    lv_style_set_bg_color(&tk_style_bar, LV_STATE_DEFAULT, get_themed_bar_background_color());
}