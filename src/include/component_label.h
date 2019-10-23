#ifndef APOLLON_COMPONENT_LABEL_H
#define APOLLON_COMPONENT_LABEL_H
#include <coelum/actor.h>
#include "actor_component.h"
#include "focus_manager.h"


typedef struct COMPONENT_LABEL_STRUCT
{
    actor_component_T base;
    char* text;
    unsigned int font_size;
    unsigned int font_spacing;
    unsigned int r;
    unsigned int g;
    unsigned int b;
} component_label_T;

component_label_T* init_component_label(focus_manager_T* focus_manager, float x, float y, float z, char* text, unsigned int r, unsigned int g, unsigned int b);

void component_label_tick(actor_T* self);

void component_label_draw(actor_T* self);
#endif
