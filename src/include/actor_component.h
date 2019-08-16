#ifndef APOLLON_ACTOR_COMPONENT_H
#define APOLLON_ACTOR_COMPONENT_H
#include <coelum/actor.h>


typedef struct ACTOR_COMPONENT_STRUCT
{
    actor_T base;
    unsigned int hovered;
    unsigned int focused;
    unsigned int visible;
    void (*on_click)(actor_T* self);
} actor_component_T;

actor_component_T* component_constructor(actor_component_T* actor_component, void (*on_click)(actor_T* self));
#endif
