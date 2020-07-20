//
// Created by riccardo on 26/06/20.
//

#ifndef TK_STYLE_H
#define TK_STYLE_H

#include "lvgl/lvgl.h"
void tk_styles_init(void);

// COLORS
#define TK_COLOR_RED_DARK LV_COLOR_MAKE(0xeb, 0x3b, 0x5a)
#define TK_COLOR_RED_LIGHT LV_COLOR_MAKE(0xfc, 0x5c, 0x65)
#define TK_COLOR_ORANGE_DARK LV_COLOR_MAKE(0xfa, 0x82, 0x31)
#define TK_COLOR_ORANGE_LIGHT LV_COLOR_MAKE(0xfd, 0x96, 0x44)
#define TK_COLOR_YELLOW_DARK LV_COLOR_MAKE(0xf7, 0xb7, 0x31)
#define TK_COLOR_YELLOW_LIGHT LV_COLOR_MAKE(0xfe, 0xd3, 0x30)
#define TK_COLOR_GREEN_DARK LV_COLOR_MAKE(0x20, 0xbf, 0x6b)
#define TK_COLOR_GREEN_LIGHT LV_COLOR_MAKE(0x26, 0xde, 0x81)
#define TK_COLOR_TURQUOISE_DARK LV_COLOR_MAKE(0x0f, 0xb9, 0xb1)
#define TK_COLOR_TURQUOISE_LIGHT LV_COLOR_MAKE(0x2b, 0xcb, 0xba)
#define TK_COLOR_AZURE_DARK LV_COLOR_MAKE(0x2d, 0x98, 0xda)
#define TK_COLOR_AZURE_LIGHT LV_COLOR_MAKE(0x45, 0xaa, 0xf2)
#define TK_COLOR_BLUE_DARK LV_COLOR_MAKE(0x38, 0x67, 0xd6)
#define TK_COLOR_BLUE_LIGHT LV_COLOR_MAKE(0x4b, 0x7b, 0xec)
#define TK_COLOR_PURPLE_DARK LV_COLOR_MAKE(0x88, 0x54, 0xd0)
#define TK_COLOR_PURPLE_LIGHT LV_COLOR_MAKE(0xa5, 0x5e, 0xea)
#define TK_COLOR_GREY_DARK LV_COLOR_MAKE(0xa5, 0xb1, 0xc2)
#define TK_COLOR_GREY_LIGHT LV_COLOR_MAKE(0xd1, 0xd8, 0xe0)
#define TK_COLOR_BLUEGRAY_DARK LV_COLOR_MAKE(0x4b, 0x65, 0x84)
#define TK_COLOR_BLUEGRAY_LIGHT LV_COLOR_MAKE(0x77, 0x8c, 0xa3)

// STYLES
lv_style_t tk_style_menu_button;
lv_style_t tk_style_menu;
lv_style_t tk_style_bar;
lv_style_t tk_style_far_background;
lv_style_t tk_style_top_bar_icon;
lv_style_t tk_style_top_bar_icon_warn;
lv_style_t tk_style_top_bar_icon_error;
lv_style_t tk_style_no_background_borders;

#endif
