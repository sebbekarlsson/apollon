#ifndef APOLLON_ACTOR_COMPONENT_H
#define APOLLON_ACTOR_COMPONENT_H
#include <coelum/actor.h>


typedef struct ACTOR_COMPONENT_STRUCT
{
    actor_T base;
    unsigned int hovered;
    unsigned int focused;
    unsigned int visible;
    unsigned int prio;
    int margin_y;
    int margin_x;
    void (*on_click)(actor_T* self);
    struct FOCUS_MANAGER_STRUCT* focus_manager;
} actor_component_T;

actor_component_T* actor_component_constructor(actor_component_T* actor_component, struct FOCUS_MANAGER_STRUCT* focus_manager, void (*on_click)(actor_T* self));

void actor_component_tick(actor_component_T* actor_component);
#endif
