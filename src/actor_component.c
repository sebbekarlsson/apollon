#include "include/actor_component.h"
#include "include/focus_manager.h"
#include <coelum/input.h>


actor_component_T* actor_component_constructor(actor_component_T* actor_component, focus_manager_T* focus_manager, void (*on_click)(actor_T* self))
{
    actor_component->on_click = on_click;
    actor_component->visible = 1;
    actor_component->hovered = 0;
    actor_component->focused = 0;
    actor_component->focus_manager = focus_manager;
    actor_component->margin_x = 0;
    actor_component->margin_y = 0;
    actor_component->prio = 0;

    return actor_component;
}

void actor_component_tick(actor_component_T* actor_component)
{
    
}
