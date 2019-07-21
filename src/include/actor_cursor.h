#ifndef APOLLON_ACTOR_CURSOR_H
#define APOLLON_ACTOR_CURSOR_H
#include <coelum/actor.h>


typedef struct ACTOR_CURSOR_STRUCT
{
    actor_T base;
    int length;
} actor_cursor_T;

actor_cursor_T* init_actor_cursor(float x, float y, float z);

void actor_cursor_tick(actor_T* self);

void actor_cursor_draw(actor_T* self);
#endif
