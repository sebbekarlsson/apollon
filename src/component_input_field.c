#include "include/component_input_field.h"
#include <coelum/draw_utils.h>
#include <coelum/current.h>
#include <coelum/input.h>
#include <coelum/constants.h>
#include <string.h>


extern keyboard_state_T* KEYBOARD_STATE;

extern const float COLOR_BG_BRIGHT[3];
extern const float COLOR_BG_DARK_BRIGHT[3];
extern const float COLOR_FG[3];

component_input_field_T* init_component_input_field(focus_manager_T* focus_manager, float x, float y, float z)
{
    component_input_field_T* input_field = calloc(1, sizeof(struct COMPONENT_INPUT_FIELD_STRUCT));
    actor_T* actor = (actor_T*) input_field;
    actor_constructor(actor, x, y, z, component_input_field_tick, component_input_field_draw, "input_field");
    actor_component_T* actor_component = (actor_component_T*) input_field;
    actor_component_constructor(actor_component, focus_manager, (void*)0);

    actor->width = 256;
    actor->height = 24;

    input_field->value = calloc(1, sizeof(char));
    input_field->value[0] = '\0';
    input_field->font_size = 6;
    input_field->font_spacing = input_field->font_size + 4; 

    input_field->caret_position = 0;
    input_field->draw_caret = 1;

    gettimeofday(&input_field->timer, 0);

    return input_field;
}

void component_input_field_draw(actor_T* self)
{
    state_T* state = get_current_state();

    component_input_field_T* input_field = (component_input_field_T*) self;
    actor_component_T* actor_component = (actor_component_T*) input_field;

    unsigned int hover = actor_component->hovered;

    draw_positioned_2D_mesh(
        self->x,
        self->y,
        self->z,
        self->width,
        self->height,
        hover ? COLOR_BG_DARK_BRIGHT[0] : COLOR_BG_BRIGHT[0],
        hover ? COLOR_BG_DARK_BRIGHT[1] : COLOR_BG_BRIGHT[1],
        hover ? COLOR_BG_DARK_BRIGHT[2] : COLOR_BG_BRIGHT[2],
        1.0f,
        state 
    );

    float scroll = 0;

    if (input_field->value)
    {
        if (strlen(input_field->value))
        {
            int available_space = self->width / (input_field->font_size + input_field->font_spacing);
            int chars_off = strlen(input_field->value) - available_space;

            scroll = chars_off > 0 ? chars_off * (input_field->font_size + input_field->font_spacing) : 0;

            glEnable(GL_SCISSOR_TEST);
            glScissor((int)self->x, (int)(RES_HEIGHT - self->y - self->height), (int)self->width, (int)self->height); 
            
            draw_text(
                input_field->value,
                self->x + (((input_field->font_size + input_field->font_spacing) / 2) - scroll),
                self->y + (self->height / 2),
                self->z,
                COLOR_FG[0],
                COLOR_FG[1],
                COLOR_FG[2],
                1.0f, // a
                input_field->font_size,
                input_field->font_spacing,
                0,
                state
            );

            glDisable(GL_SCISSOR_TEST);
        }
    }

    if (input_field->draw_caret && actor_component->focused)
    {
        draw_positioned_2D_mesh(
            self->x + ((input_field->font_size + input_field->font_spacing) * (float) input_field->caret_position - ((input_field->font_size + input_field->font_spacing) / 2)) + ((input_field->font_size + input_field->font_spacing) / 2) - scroll,
            self->y - (input_field->font_size) + (self->height / 2),
            self->z + 0.1f,
            4,
            (input_field->font_size * 2),
            COLOR_FG[0],
            COLOR_FG[1],
            COLOR_FG[2],
            1.0f,
            state       
        );
    } 

    draw_positioned_2D_mesh(
        self->x,
        self->y + self->height,
        self->z,
        self->width,
        4,
        COLOR_FG[0],
        COLOR_FG[1],
        COLOR_FG[2],
        1.0f,
        state 
    );
}

void component_input_field_tick(actor_T* self)
{
    component_input_field_T* input_field = (component_input_field_T*) self;
    actor_component_T* actor_component = (actor_component_T*) input_field;

    actor_component_tick(actor_component);

    if (!actor_component->focused)
        return;

    if (input_field->value)
        input_field->caret_position = (int) strlen(input_field->value);
    else
        input_field->caret_position = 0;

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
        input_field->draw_caret = input_field->draw_caret ? 0 : 1;

        gettimeofday(&input_field->timer, 0);
    }
}
