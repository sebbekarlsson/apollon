#ifndef APOLLON_COMPONENT_TEXTAREA_H
#define APOLLON_COMPONENT_TEXTAREA_H
#include "component_textable.h"


typedef struct TEXTAREA_STRUCT
{
    component_textable_T base;
} component_textarea_T;

component_textarea_T* init_component_textarea(focus_manager_T* focus_manager, float x, float y, int width, int height);

void component_textarea_tick(actor_T* self);

void component_textarea_draw(actor_T* self);
#endif
