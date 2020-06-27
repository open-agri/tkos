//
// Created by riccardo on 26/06/20.
//

#include "tk_style.h"
#include "malloc.h"

void tk_styles_init(void) {

  // MENU BUTTON
  lv_style_init(&tk_style_menu_button);
  lv_style_set_bg_opa(&tk_style_menu_button, LV_STATE_DEFAULT, LV_OPA_TRANSP);
  lv_style_set_bg_opa(&tk_style_menu_button, LV_STATE_FOCUSED, LV_OPA_100);
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
  lv_style_set_bg_color(&tk_style_menu, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_style_set_radius(&tk_style_menu, LV_STATE_DEFAULT, 0);
  lv_style_set_border_width(&tk_style_menu, LV_STATE_DEFAULT, 0);
  lv_style_set_outline_width(&tk_style_menu, LV_STATE_DEFAULT, 0);
  lv_style_set_pad_left(&tk_style_menu, LV_STATE_DEFAULT, 0);
  lv_style_set_pad_right(&tk_style_menu, LV_STATE_DEFAULT, 0);
  lv_style_set_line_width(&tk_style_menu, LV_STATE_DEFAULT, 0);


  // BOTTOM BAR
  lv_style_init(&tk_style_bb);
  lv_style_set_shadow_color(&tk_style_bb, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_style_set_shadow_opa(&tk_style_bb, LV_STATE_DEFAULT, LV_OPA_70);
  lv_style_set_shadow_spread(&tk_style_bb, LV_STATE_DEFAULT, 20);
  lv_style_set_shadow_ofs_x(&tk_style_bb, LV_STATE_DEFAULT, 16);
	lv_style_set_radius(&tk_style_bb, LV_STATE_DEFAULT, 4);
	lv_style_set_border_width(&tk_style_bb, LV_STATE_DEFAULT, 0);
  lv_style_set_clip_corner(&tk_style_bb, LV_STATE_DEFAULT, true);
}