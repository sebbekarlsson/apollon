#include "include/actor_focusable.h"
#include <coelum/input.h>


extern mouse_state_T* MOUSE_STATE;


actor_focusable_T* actor_focusable_constructor(actor_focusable_T* actor_focusable)
{
    actor_focusable->focused = 0;
    actor_focusable->hover = 0;
    actor_focusable->visible = 1;

    return actor_focusable;
}

void actor_focusable_tick(actor_focusable_T* actor_focusable)
{
    actor_T* actor = (actor_T*) actor_focusable;

    actor_focusable->hover = 0;

    if (MOUSE_STATE->x >= actor->x && MOUSE_STATE->x <= actor->x + actor->width)
    {
        if (MOUSE_STATE->y >= actor->y && MOUSE_STATE->y <= actor->y + actor->height)
        {
            actor_focusable->hover = 1;
        }
    }
}
