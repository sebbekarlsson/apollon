#ifndef APOLLON_COMPONENT_INPUT_FIELD_H
#define APOLLON_COMPONENT_INPUT_FIELD_H
#include "actor_component.h"
#include "focus_manager.h"
#include <sys/time.h>
#include <glad/glad.h>


typedef struct COMPONENT_INPUT_FIELD_STRUCT
{
    actor_component_T base;
    char* value;
    unsigned int caret_position;
    unsigned int font_size;
    unsigned int font_spacing;
    unsigned int draw_caret;
    struct timeval timer;
} component_input_field_T;

component_input_field_T* init_component_input_field(focus_manager_T* focus_manager, float x, float y, float z);

void component_input_field_draw(actor_T* self);

void component_input_field_tick(actor_T* self);
#endif
