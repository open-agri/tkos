#include "views.h"

typedef struct tk_view_stack_item
{
    char *descriptor;
    struct tk_view_stack_item *previous;
    tk_view_generator generator;
} tk_view_stack_item;

static tk_view_stack_item *view_stack_last = NULL;

void view_navigate(tk_view_generator generator, bool record_stack)
{
    // Save old screen
    lv_obj_t *old_view_content = lv_scr_act();

    // Generate tk view
    tk_view view = (generator)();

    // Get configuration
    tk_bottom_bar_configuration bar_conf = view.bottom_bar_configuration;

    // Get new screen (copy, otherwise gets corrupted)
    lv_obj_t *view_content = view.content;

    // Put new screen in stack
    if (record_stack)
    {
        tk_view_stack_item new_item;
        new_item.generator = generator;
        new_item.previous = view_stack_last;

        view_stack_last = &new_item;
    }

    // TODO: Destroy old bars?

    // Show new screen
    lv_scr_load(view_content);

    // TODO: Draw bottom/top bar
    lv_obj_t* bottom_bar = build_bottom_bar(bar_conf);
    lv_obj_align(bottom_bar, view_content, LV_ALIGN_IN_BOTTOM_MID, 0, 0);


    // Destroy old screen    lv_obj_t* bottom_bar = build_bottom_bar(&(view->bottom_bar_configuration));
    if (old_view_content != NULL)
        lv_obj_del(old_view_content);
}

void view_navigate_back()
{
    // Pop from the stack
    tk_view_stack_item *popped_item = view_stack_last;

    // Exit if root
    if (popped_item->previous == NULL)
        return;

    // Update stack top
    view_stack_last = popped_item->previous;

    // Navigate
    view_navigate(popped_item->previous->generator, false);

    // Free
    free(popped_item);
}