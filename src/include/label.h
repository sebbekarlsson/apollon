#ifndef APOLLON_LABEL_H
#define APOLLON_LABEL_H
#include <coelum/actor.h>


typedef struct LABEL_STRUCT
{
    actor_T base;
    char* text;
    unsigned int font_size;
    unsigned int font_spacing;
    unsigned int r;
    unsigned int g;
    unsigned int b;
} label_T;

label_T* init_label(float x, float y, float z, char* text);

void label_tick(actor_T* self);

void label_draw(actor_T* self);
#endif
