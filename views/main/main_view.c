#include "../views.h"

static void button_event_callback(lv_obj_t *obj, lv_event_t event) {
  switch (event) {
	// case LV_EVENT_CLICKED:view_navigate(build_settings_info_view, true);
	  break;
  }
}

tk_view build_main_view() {

  // Content
  lv_obj_t *main_view_content = lv_obj_create(NULL, NULL);

  lv_obj_t *label = lv_label_create(main_view_content, NULL);
  lv_label_set_text(label, "Main screen");
  lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);

  lv_obj_t *button = lv_btn_create(main_view_content, NULL);
  lv_obj_t *button_label = lv_label_create(button, NULL);
  lv_label_set_text(button_label, "About this device");
  lv_obj_align(button, label, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
  lv_obj_set_event_cb(button, button_event_callback);

  // Bottom bar
  tk_bottom_bar_button right = {
	  .text = "Menu", .click_callback = button_event_callback};

  tk_bottom_bar_configuration bb_conf = {
	  .right_button = right, .center_symbol = "TEST"};

  // Return struct
  tk_view main_view = {
	  .content = main_view_content,
	  .bottom_bar_configuration = bb_conf
  };

  return main_view;
}
