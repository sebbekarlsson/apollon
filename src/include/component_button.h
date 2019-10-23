#ifndef APOLLON_COMPONENT_BUTTON_H
#define APOLLON_COMPONENT_BUTTON_H
#include "actor_component.h"
#include "focus_manager.h"


typedef struct COMPONENT_BUTTON_STRUCT
{
    actor_component_T base;
    char* text;
    unsigned int fg_r;
    unsigned int fg_g;
    unsigned int fg_b;
    unsigned int bg_r;
    unsigned int bg_g;
    unsigned int bg_b;
    float alpha;
    unsigned int font_size;
    unsigned int font_spacing;
    unsigned int disabled;
    void (*press)(struct COMPONENT_BUTTON_STRUCT* self);
} component_button_T;

component_button_T* init_component_button(focus_manager_T* focus_manager, float x, float y, float z, char* text, void (*press)());

void component_button_tick(actor_T* self);

void component_button_draw(actor_T* self);

void component_button_free(component_button_T* button);
#endif
