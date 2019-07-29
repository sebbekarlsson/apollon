#ifndef APOLLON_MODAL_H
#define APOLLON_MODAL_H
#include <coelum/actor.h>
#include "button.h"
#include "focus_manager.h"


typedef struct MODAL_STRUCT
{
    actor_T base;
    unsigned int width;
    unsigned int height;
    char* title;
    char* text;
    button_T* button;
    focus_manager_T* focus_manager;
} modal_T;

modal_T* init_modal(float x, float y, char* title, char* text, state_T* state);

void modal_tick(actor_T* self);

void modal_draw(actor_T* self);

void modal_free(modal_T* modal);
#endif
