#ifndef APOLLON_BUTTON_H
#define APOLLON_BUTTON_H
#include "actor_focusable.h"


typedef struct BUTTON_STRUCT
{
    actor_focusable_T base;
    char* text;
    unsigned int width;
    unsigned int height;
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
    void (*press)();
} button_T;

button_T* init_button(float x, float y, float z, char* text, void (*press)());

void button_tick(actor_T* self);

void button_draw(actor_T* self);

void button_free(button_T* button);
#endif
