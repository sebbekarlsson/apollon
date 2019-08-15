#ifndef APOLLON_ACTOR_FOCUSABLE_H
#define APOLLON_ACTOR_FOCUSABLE_H
#include <coelum/actor.h>


typedef struct ACTOR_FOCUSABLE_STRUCT
{
    actor_T base;
    unsigned int focused;
    unsigned int hover;
    unsigned int visible;
} actor_focusable_T;


actor_focusable_T* actor_focusable_constructor(actor_focusable_T* actor_focusable);

void actor_focusable_tick(actor_focusable_T* actor_focusable);
#endif
