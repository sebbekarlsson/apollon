#include "include/input_field.h"
#include <coelum/draw_utils.h>
#include <coelum/current.h>
#include <coelum/input.h>
#include <string.h>


extern keyboard_state_T* KEYBOARD_STATE;

input_field_T* init_input_field(float x, float y, float z)
{
    input_field_T* input_field = calloc(1, sizeof(struct INPUT_FIELD_STRUCT));
    actor_T* actor = (actor_T*) input_field;
    actor_constructor(actor, x, y, z, input_field_tick, input_field_draw, "input_field");

    input_field->focused = 0;
    input_field->value = '\0';

    input_field->caret_position = 0;

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

    draw_positioned_2D_mesh(
        self->x + ((12 + 16) * (float) input_field->caret_position - ((12 + 16) / 2)),
        self->y - 12,
        self->z,
        4,
        (12 * 2),
        0,
        0,
        0,
        state        
    );
}

void input_field_tick(actor_T* self)
{
    input_field_T* input_field = (input_field_T*) self;

    if (!input_field->focused)
        return;

    input_field->caret_position = (int) strlen(input_field->value);

    if (KEYBOARD_STATE->keys[GLFW_KEY_BACKSPACE] && !KEYBOARD_STATE->key_locks[GLFW_KEY_BACKSPACE])
    {
        if (strlen(input_field->value) > 0)
            input_field->value[strlen(input_field->value) - 1] ='\0';
        
        KEYBOARD_STATE->key_locks[GLFW_KEY_BACKSPACE] = 1;
    }
    else
    { // alright, we are typing something.
        char* char_str = calloc(2, sizeof(char));
        char_str[0] = KEYBOARD_STATE->character;
        char_str[1] = '\0';

        strcat(input_field->value, char_str);
    }
}
