#include "include/input_field.h"
#include <coelum/draw_utils.h>
#include <coelum/current.h>
#include <coelum/input.h>
#include <coelum/constants.h>
#include <string.h>


extern keyboard_state_T* KEYBOARD_STATE;

extern const float COLOR_BG_BRIGHT[3];

input_field_T* init_input_field(float x, float y, float z)
{
    input_field_T* input_field = calloc(1, sizeof(struct INPUT_FIELD_STRUCT));
    actor_T* actor = (actor_T*) input_field;
    actor_constructor(actor, x, y, z, input_field_tick, input_field_draw, "input_field");
    actor_focusable_T* actor_focusable = (actor_focusable_T*) input_field;
    actor_focusable_constructor(actor_focusable);

    input_field->value = calloc(1, sizeof(char));
    input_field->value[0] = '\0';
    input_field->font_size = 8;
    input_field->font_spacing = input_field->font_size + 4;
    input_field->width = 256;
    input_field->height = 32;

    input_field->caret_position = 0;
    input_field->draw_caret = 1;

    gettimeofday(&input_field->timer, 0);

    return input_field;
}

void input_field_draw(actor_T* self)
{
    input_field_T* input_field = (input_field_T*) self;
    actor_focusable_T* actor_focusable = (actor_focusable_T*) input_field;
    state_T* state = (state_T*) get_current_scene();

    draw_positioned_2D_mesh(
        self->x,
        self->y,
        self->z,
        input_field->width,
        input_field->height,
        COLOR_BG_BRIGHT[0],
        COLOR_BG_BRIGHT[1],
        COLOR_BG_BRIGHT[2],
        state 
    );

    float scroll = 0;

    if (input_field->value)
    {
        if (strlen(input_field->value))
        {
            int available_space = input_field->width / (input_field->font_size + input_field->font_spacing);
            int chars_off = strlen(input_field->value) - available_space;

            scroll = chars_off > 0 ? chars_off * (input_field->font_size + input_field->font_spacing) : 0;

            glEnable(GL_SCISSOR_TEST);
            glScissor((int)self->x, (int)(WINDOW_HEIGHT - self->y - input_field->height), (int)input_field->width, (int)input_field->height); 
            
            draw_text(
                input_field->value,
                self->x + (((input_field->font_size + input_field->font_spacing) / 2) - scroll),
                self->y + (input_field->height / 2),
                self->z,
                0,
                0,
                0,
                input_field->font_size,
                input_field->font_spacing,
                0,
                state
            );
            glDisable(GL_SCISSOR_TEST);
        }
    }

    if (input_field->draw_caret && actor_focusable->focused)
    {
        draw_positioned_2D_mesh(
            self->x + ((input_field->font_size + input_field->font_spacing) * (float) input_field->caret_position - ((input_field->font_size + input_field->font_spacing) / 2)) + ((input_field->font_size + input_field->font_spacing) / 2) - scroll,
            self->y - (input_field->font_size) + (input_field->height / 2),
            self->z,
            4,
            (input_field->font_size * 2),
            0,
            0,
            0,
            state        
        );
    } 

    draw_positioned_2D_mesh(
        self->x,
        self->y + input_field->height,
        self->z,
        input_field->width,
        4,
        0,
        0,
        0,
        state 
    );
}

void input_field_tick(actor_T* self)
{
    input_field_T* input_field = (input_field_T*) self;
    actor_focusable_T* actor_focusable = (actor_focusable_T*) input_field;

    if (!actor_focusable->focused)
    {
        return;
    }

    if (input_field->value)
    {
        input_field->caret_position = (int) strlen(input_field->value);
    }
    else
    {
        input_field->caret_position = 0;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_BACKSPACE] && !KEYBOARD_STATE->key_locks[GLFW_KEY_BACKSPACE])
    {
        if (strlen(input_field->value) > 0)
            input_field->value[strlen(input_field->value) - 1] ='\0';
        
        KEYBOARD_STATE->key_locks[GLFW_KEY_BACKSPACE] = 1;
    }
    else if (KEYBOARD_STATE->character)
    { // alright, we are typing something.
        char* char_str = calloc(2, sizeof(char));
        char_str[0] = KEYBOARD_STATE->character;
        char_str[1] = '\0';

        input_field->value = realloc(input_field->value, strlen(input_field->value) + 2);

        strcat(input_field->value, char_str);
    }

    struct timeval end;
    gettimeofday(&end, 0);

    float time_spent = (double)(end.tv_usec - input_field->timer.tv_usec) / 1000000 + (double)(end.tv_sec - input_field->timer.tv_sec);

    if (time_spent >= 0.5f)
    {
        if (input_field->draw_caret)
        {
            input_field->draw_caret = 0;
        }
        else
        {
            input_field->draw_caret = 1;
        }

        gettimeofday(&input_field->timer, 0);
    }
}
