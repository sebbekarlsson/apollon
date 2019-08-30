#ifndef APOLLON_MODAL_H
#define APOLLON_MODAL_H
#include <coelum/actor.h>
#include "button.h"
#include "component_pane.h"
#include "component_button.h"


typedef struct MODAL_STRUCT
{
    actor_T base;
    unsigned int width;
    unsigned int height;
    char* title;
    char* text;
    component_pane_T* component_pane;
    component_button_T* component_button;
} modal_T;

modal_T* init_modal(float x, float y, char* title, char* text, state_T* state, focus_manager_T* focus_manager);

void modal_tick(actor_T* self);

void modal_draw(actor_T* self);

void modal_free(modal_T* modal);
#endif
