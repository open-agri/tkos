#include "settings_info_view.h"

tk_view *build_settings_info_view()
{
    lv_obj_t *settings_info_view = lv_obj_create(NULL, NULL);

    lv_obj_t *label = lv_label_create(settings_info_view, NULL);
    lv_label_set_text(label, "OpenAgri TractorKit Commander\n OS Version (unknown), bootloader is unlocked.");
    lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_MID, 0, 40);

    return settings_info_view;
}