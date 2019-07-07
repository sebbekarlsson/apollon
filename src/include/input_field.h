#ifndef APOLLON_INPUT_FIELD_H
#define APOLLON_INPUT_FIELD_H
#include <coelum/actor.h>


typedef struct INPUT_FIELD_STRUCT
{
    actor_T base;
    char* value;
    unsigned int focused;
} input_field_T;

input_field_T* init_input_field(float x, float y, float z);

void input_field_draw(actor_T* self);

void input_field_tick(actor_T* self);
#endif
