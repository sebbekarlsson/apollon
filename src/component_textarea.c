#include "include/component_textarea.h"


component_textarea_T* init_component_textarea(focus_manager_T* focus_manager, float x, float y, int width, int height)
{
    component_textarea_T* textarea = calloc(1, sizeof(struct TEXTAREA_STRUCT));

    component_textable_constructor(
        focus_manager,
        (component_textable_T*) textarea,
        x,
        y,
        width,
        height,
        1
    );

    return textarea;
}
