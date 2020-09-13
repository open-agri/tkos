/**
 * @file viewnavigator.c
 * @author Riccardo Persello (riccardo.persello@icloud.com)
 * @brief View navigator.
 * @version 0.1
 * @date 2020-09-11
 * 
 * 
 */

#include "views.h"
#include "esp_log.h"

#define TAG "View navigator"

/**
 * @brief An item of the internal view stack.
 * 
 */
typedef struct tk_view_stack_item
{
    struct tk_view_stack_item *previous;
    tk_view_generator generator;
} tk_view_stack_item;

static tk_view_stack_item *view_stack_last = NULL;

/**
 * @brief Navigates to the view defined by its generator, and optionally adds it to the navigation stack.
 * 
 * @param generator The generator for the new view.
 * @param record_stack Record this navigation in the stack if set to true.
 */
void view_navigate(tk_view_generator generator, bool record_stack)
{

    ESP_LOGD(TAG, "Navigating %srecording stack.", record_stack ? "" : "without ");

    // Save old screen
    lv_obj_t *old_view_content = lv_scr_act();

    // Generate tk view
    tk_view_t view = (generator)();

    // Get configuration
    tk_bottom_bar_configuration_t bar_conf = view.bottom_bar_configuration;

    // Get new screen (copy, otherwise gets corrupted)
    current_view_content = view.content;

    // Put new screen in stack
    if (record_stack)
    {
        tk_view_stack_item *new_item = (tk_view_stack_item*) malloc(sizeof(tk_view_stack_item));
        new_item->generator = generator;
        new_item->previous = view_stack_last;

        view_stack_last = new_item;
    }

    // Show new screen
    lv_scr_load(current_view_content);

    // Draw bottom bar
    lv_obj_t *bottom_bar = build_bottom_bar(bar_conf, true);
    lv_obj_align(bottom_bar, lv_scr_act(), LV_ALIGN_IN_BOTTOM_MID, 0, 0);

    // Destroy old screen
    if (old_view_content != NULL)
        lv_obj_del(old_view_content);
}

/**
 * @brief Pops an item from the navigation stack, generates its view and shows its content on screen.
 * 
 */
void view_navigate_back()
{
    ESP_LOGD(TAG, "Popping view stack.");
    
    // Pop from the stack
    tk_view_stack_item *popped_item = view_stack_last;

    // Exit if root
    if (popped_item == NULL || popped_item->previous == NULL)
        return;

    // Update stack top
    view_stack_last = popped_item->previous;

    // Navigate
    view_navigate(popped_item->previous->generator, false);

    // Free
    free(popped_item);
}