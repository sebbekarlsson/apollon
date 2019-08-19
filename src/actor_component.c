#include "include/actor_component.h"
#include <coelum/input.h>


extern mouse_state_T* MOUSE_STATE;


actor_component_T* component_constructor(actor_component_T* actor_component, void (*on_click)(actor_T* self))
{
    actor_component->on_click = on_click;
    actor_component->visible = 1;
    actor_component->hovered = 0;
    actor_component->focused = 0;

    return actor_component;
}

void actor_component_tick(actor_component_T* actor_component)
{
    actor_T* actor = (actor_T*) actor_component;

    actor_component->hovered = 0;

    if (MOUSE_STATE->x >= actor->x && MOUSE_STATE->x <= actor->x + actor->width)
    {
        if (MOUSE_STATE->y >= actor->y && MOUSE_STATE->y <= actor->y + actor->height)
        {
            actor_component->hovered = 1;
        }
    }
}
