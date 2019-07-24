#ifndef APOLLON_CHECKBOX_H
#define APOLLON_CHECKBOX_H
#include "actor_focusable.h"

typedef struct CHECKBOX_STRUCT
{
    actor_focusable_T base;
    unsigned int checked;
    unsigned int width;
    unsigned int height;
} checkbox_T;

checkbox_T* init_checkbox(
    float x,
    float y,
    float z    
);

void checkbox_tick(actor_T* self);

void checkbox_draw(actor_T* self);
#endif
