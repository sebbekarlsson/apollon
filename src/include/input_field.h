#ifndef APOLLON_INPUT_FIELD_H
#define APOLLON_INPUT_FIELD_H
#include "actor_focusable.h"
#include <sys/time.h>
#include <glad/glad.h>


typedef struct INPUT_FIELD_STRUCT
{
    actor_focusable_T base;
    char* value;
    unsigned int caret_position;
    unsigned int font_size;
    unsigned int font_spacing;
    unsigned int draw_caret;
    int width;
    int height;
    struct timeval timer;
} input_field_T;

input_field_T* init_input_field(float x, float y, float z);

void input_field_draw(actor_T* self);

void input_field_tick(actor_T* self);
#endif
