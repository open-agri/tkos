//
// Created by riccardo on 26/06/20.
//

#include "tk_style.h"
#include "malloc.h"

void tk_styles_init(void) {

  // MENU BUTTON
  lv_style_init(&tk_style_menu_button);
  lv_style_set_bg_color(&tk_style_menu_button, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_style_set_bg_color(&tk_style_menu_button, LV_STATE_PRESSED, LV_THEME_DEFAULT_COLOR_PRIMARY);
  lv_style_set_radius(&tk_style_menu_button, LV_STATE_DEFAULT, 0);
  lv_style_set_border_width(&tk_style_menu_button, LV_STATE_DEFAULT, 0);
  lv_style_set_outline_width(&tk_style_menu_button, LV_STATE_DEFAULT, 0);

}