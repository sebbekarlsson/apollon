#include "include/input_field.h"
#include <coelum/draw_utils.h>
#include <coelum/current.h>


input_field_T* init_input_field(float x, float y, float z)
{
    input_field_T* input_field = calloc(1, sizeof(struct INPUT_FIELD_STRUCT));
    actor_T* actor = (actor_T*) input_field;
    actor_constructor(actor, x, y, z, input_field_tick, input_field_draw, "input_field");

    input_field->focused = 0;
    input_field->value = '\0';

    return input_field;
}

void input_field_draw(actor_T* self)
{
    input_field_T* input_field = (input_field_T*) self;
    state_T* state = (state_T*) get_current_scene();

    draw_text(
        input_field->value,
        self->x,
        self->y,
        self->z,
        0,
        0,
        0,
        12,
        16,
        state
    );
}

void input_field_tick(actor_T* self)
{
}
