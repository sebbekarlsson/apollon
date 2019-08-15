#include "include/textarea.h"


textarea_T* init_textarea(float x, float y, int width, int height)
{
    textarea_T* textarea = calloc(1, sizeof(struct TEXTAREA_STRUCT));

    actor_textable_constructor(
        (actor_textable_T*) textarea,
        x,
        y,
        width,
        height,
        1
    );

    return textarea;
}
