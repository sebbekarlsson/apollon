#include "include/actor_component.h" 


actor_component_T* component_constructor(actor_component_T* actor_component, void (*on_click)(actor_T* self))
{
    actor_component->on_click = on_click;

    return actor_component;
}
