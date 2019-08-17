#include "include/actor_component.h" 


actor_component_T* component_constructor(actor_component_T* actor_component, void (*on_click)(actor_T* self))
{
    actor_component->on_click = on_click;
    actor_component->visible = 1;
    actor_component->hovered = 0;
    actor_component->focused = 0;

    return actor_component;
}
