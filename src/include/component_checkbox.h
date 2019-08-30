#ifndef APOLLON_COMPONENT_CHECKBOX_H
#define APOLLON_COMPONENT_CHECKBOX_H
#include "actor_component.h"
#include "focus_manager.h"


typedef struct COMPONENT_CHECKBOX_STRUCT
{
    actor_component_T base;
    unsigned int checked;
} component_checkbox_T;

component_checkbox_T* init_component_checkbox(
    focus_manager_T* focus_manager,
    float x,
    float y,
    float z    
);

void component_checkbox_tick(actor_T* self);

void component_checkbox_draw(actor_T* self);
#endif
