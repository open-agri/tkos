/**
 * @file tk_style.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief Styles for TractorKit.
 * @version 0.1
 * @date 2020-09-07
 * 
 * 
 */

#include "tk_style.h"
#include <stdlib.h>

#undef LV_THEME_DEFAULT_COLOR_PRIMARY
#undef LV_THEME_DEFAULT_COLOR_SECONDARY
#define LV_THEME_DEFAULT_COLOR_PRIMARY tk_get_primary_color()
#define LV_THEME_DEFAULT_COLOR_SECONDARY tk_get_secondary_color()

// TODO: Implement accent changing
/**
 * @brief Gets the primary color based on the current theme and color settings.
 * 
 * @return lv_color_t The primary color.
 */
lv_color_t tk_get_primary_color() {
    return LV_THEME_MATERIAL_FLAG_DARK ? TK_COLOR_ORANGE_DARK : TK_COLOR_ORANGE_LIGHT;
}

/**
 * @brief Gets the secondary color based on the current theme and color settings.
 * 
 * @return lv_color_t The secondary color.
 */
lv_color_t tk_get_secondary_color() {
    return LV_THEME_MATERIAL_FLAG_DARK ? TK_COLOR_RED_DARK : TK_COLOR_RED_LIGHT;
}

/**
 * @brief Set the default shadow for a style.
 * 
 * @param style The style on which to apply this shadow.
 * @param height The height of the object on an imaginary z-axis coming out of the screen.
 */
void tk_set_default_shadow(lv_style_t *style, int height)
{
    int radius = height * height / 2;
    lv_style_set_shadow_color(style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_shadow_opa(style, LV_STATE_DEFAULT,
                            LV_THEME_DEFAULT_FLAG == LV_THEME_MATERIAL_FLAG_DARK ? LV_OPA_30 : LV_OPA_40);
    lv_style_set_shadow_ofs_y(style, LV_STATE_DEFAULT, 0);
    lv_style_set_shadow_width(style, LV_STATE_DEFAULT, radius);
}

/**
 * @brief Get the bar background color for the current theme settings.
 * 
 * @return lv_color_t The bar background color.
 */
lv_color_t tk_get_themed_bar_background_color()
{
    if (LV_THEME_DEFAULT_FLAG == LV_THEME_MATERIAL_FLAG_DARK)
    {
        return LV_COLOR_MAKE(24, 24, 24);
    }
    else
    {
        return LV_COLOR_MAKE(237, 237, 237);
    }
}

/**
 * @brief Get the background color for far items for the current theme settings.
 * 
 * @return lv_color_t The background color.
 */
lv_color_t tk_get_themed_far_background_color()
{
    if (LV_THEME_DEFAULT_FLAG == LV_THEME_MATERIAL_FLAG_DARK)
    {
        return LV_COLOR_MAKE(8, 8, 8);
    }
    else
    {
        return LV_COLOR_WHITE;
    }
}

/**
 * @brief Get the background color for near items for the current theme settings.
 * 
 * @return lv_color_t The background color.
 */
lv_color_t tk_get_themed_near_background_color()
{
    lv_color_t color = tk_get_themed_bar_background_color();
    int mix;
    if (LV_THEME_DEFAULT_FLAG == LV_THEME_MATERIAL_FLAG_DARK)
    {
        mix = 230;
    }
    else
    {
        mix = 100;
    }
    return lv_color_mix(color, LV_COLOR_WHITE, mix);
}

// TODO: Call on theme change
/**
 * @brief Initializes all the TractorKit styles. This function should be called before drawing any UI.
 * 
 */
void tk_styles_init(void)
{
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
    tk_set_default_shadow(&tk_style_menu, 9);
    lv_style_set_radius(&tk_style_menu, LV_STATE_DEFAULT, 8);
    lv_style_set_clip_corner(&tk_style_menu, LV_STATE_DEFAULT, true);
    lv_style_set_border_width(&tk_style_menu, LV_STATE_DEFAULT, 0);
    lv_style_set_outline_width(&tk_style_menu, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_left(&tk_style_menu, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_right(&tk_style_menu, LV_STATE_DEFAULT, 0);
    lv_style_set_line_width(&tk_style_menu, LV_STATE_DEFAULT, 0);
    lv_style_set_bg_color(&tk_style_menu, LV_STATE_DEFAULT, tk_get_themed_near_background_color());

    // FULLSCREEN MENU
    lv_style_init(&tk_style_menu_fullscreen);
    lv_style_copy(&tk_style_menu_fullscreen, &tk_style_menu);
    lv_style_set_radius(&tk_style_menu_fullscreen, LV_STATE_DEFAULT, 0);

    // BACKGROUND
    lv_style_init(&tk_style_far_background);
    lv_style_set_bg_color(&tk_style_far_background, LV_STATE_DEFAULT, tk_get_themed_far_background_color());

    // BARS
    lv_style_init(&tk_style_bar);
    tk_set_default_shadow(&tk_style_bar, 7);
    lv_style_set_radius(&tk_style_bar, LV_STATE_DEFAULT, 0);
    lv_style_set_border_width(&tk_style_bar, LV_STATE_DEFAULT, 0);
    lv_style_set_clip_corner(&tk_style_bar, LV_STATE_DEFAULT, true);
    lv_style_set_bg_color(&tk_style_bar, LV_STATE_DEFAULT, tk_get_themed_bar_background_color());

    // ICON (NORMAL)
    lv_style_init(&tk_style_top_bar_icon);
    lv_style_set_text_font(&tk_style_top_bar_icon, LV_STATE_DEFAULT, &icons_16);
    lv_style_set_text_color(&tk_style_top_bar_icon, LV_STATE_DEFAULT, (LV_THEME_DEFAULT_FLAG == LV_THEME_MATERIAL_FLAG_DARK) ? LV_COLOR_WHITE : LV_COLOR_BLACK);

    // ICON (WARNING)
    lv_style_init(&tk_style_top_bar_icon_warn);
    lv_style_set_text_font(&tk_style_top_bar_icon_warn, LV_STATE_DEFAULT, &icons_16);
    lv_style_set_text_color(&tk_style_top_bar_icon_warn, LV_STATE_DEFAULT, (LV_THEME_DEFAULT_FLAG == LV_THEME_MATERIAL_FLAG_DARK) ? TK_COLOR_YELLOW_LIGHT : TK_COLOR_YELLOW_DARK);

    // ICON (ERROR)
    lv_style_init(&tk_style_top_bar_icon_error);
    lv_style_set_text_font(&tk_style_top_bar_icon_error, LV_STATE_DEFAULT, &icons_16);
    lv_style_set_text_color(&tk_style_top_bar_icon_error, LV_STATE_DEFAULT, (LV_THEME_DEFAULT_FLAG == LV_THEME_MATERIAL_FLAG_DARK) ? TK_COLOR_RED_LIGHT : TK_COLOR_RED_DARK);

    // NO BACKGROUND AND BORDERS
    lv_style_init(&tk_style_no_background_borders);
    lv_style_set_bg_opa(&tk_style_no_background_borders, LV_STATE_DEFAULT, LV_OPA_0);
    lv_style_set_border_width(&tk_style_no_background_borders, LV_STATE_DEFAULT, 0);
}