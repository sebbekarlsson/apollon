#ifndef APOLLON_TEXTAREA_H
#define APOLLON_TEXTAREA_H
#include "actor_textable.h"


typedef struct TEXTAREA_STRUCT
{
    actor_textable_T base;
} textarea_T;

textarea_T* init_textarea(float x, float y, int width, int height);

void textarea_tick(actor_T* self);

void textarea_draw(actor_T* self);
#endif
