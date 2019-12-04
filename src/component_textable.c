#include "include/component_textable.h"
#include "include/string_helpers.h"
#include <string.h>
#include <coelum/draw_utils.h>
#include <coelum/current.h>
#include <coelum/constants.h>
#include <coelum/input.h>
#include <hermes/string_utils.h>


extern keyboard_state_T* KEYBOARD_STATE;
extern const float COLOR_CONTRAST[3];


component_textable_T* component_textable_constructor(
    focus_manager_T* focus_manager,
    component_textable_T* component_textable,
    float x,
    float y,
    int width,
    int height,
    unsigned int supports_multiple_lines
)
{
    actor_component_T* actor_component = actor_component_constructor(
        (actor_component_T*) component_textable,
        focus_manager,
        (void*)0
    );

    actor_T* actor = actor_constructor(
        (actor_T*) actor_component,
        x,
        y,
        0.0f,
        component_textable_tick,
        component_textable_draw,
        "component_textable"
    );

    actor->width = (float)width;
    actor->height = (float)height;

    component_textable->supports_multiple_lines = supports_multiple_lines;
    component_textable->fg_r = COLOR_CONTRAST[0];
    component_textable->fg_g = COLOR_CONTRAST[1];
    component_textable->fg_b = COLOR_CONTRAST[2];
    component_textable->bg_r = 0;
    component_textable->bg_g = 0;
    component_textable->bg_b = 0;
    component_textable->caret_position = 0;
    component_textable->draw_caret = 1;
    component_textable->font_size = 8;
    component_textable->font_spacing = 8;
    component_textable->value = calloc(1, sizeof(char));
    component_textable->value[0] = '\0';
    component_textable->caret_x = 0;
    component_textable->caret_y = 0;

    gettimeofday(&component_textable->timer, 0);

    return component_textable;
}

void component_textable_tick(actor_T* self)
{
    component_textable_T* component_textable = (component_textable_T*) self;
    actor_component_T* actor_component = (actor_component_T*) component_textable;

    if (!actor_component->focused)
        return;

    while(component_textable->value[component_textable->caret_position] == '\n')
        component_textable->caret_position -= 1;

    component_textable_caret_blink(component_textable);
    component_textable_handle_keyboard_input(component_textable);
}

void component_textable_draw(actor_T* self)
{
    component_textable_T* component_textable = (component_textable_T*) self;

    state_T* state = get_current_state();

    draw_positioned_2D_mesh(
        self->x,
        self->y,
        self->z,
        self->width,
        self->height,
        component_textable->bg_r,
        component_textable->bg_g,
        component_textable->bg_b,
        1.0f,
        state       
    );

    component_textable_draw_text_value(component_textable);
    component_textable_draw_caret(component_textable);
}

void component_textable_draw_text_value(component_textable_T* self)
{
    actor_component_T* actor_component = (actor_component_T*) self;
    actor_T* actor = (actor_T*) actor_component;

    if (self->value == (void*) 0)
        return;

    if (strlen(self->value) == 0)
        return;

    state_T* state = get_current_state();

    int scroll = component_textable_calculate_scroll(self);

    glEnable(GL_SCISSOR_TEST);
    glScissor((int)actor->x, (int)(RES_HEIGHT - actor->y - actor->height), (int)actor->width, (int)actor->height);

    int line = 0;
    char val[1024];
    strcpy(val, self->value);

    char* ptr = strtok(val, "\r");

    if (val[0] == '\r')
        line += 1;

    while (ptr)
    {
        draw_text(
            ptr,
            4 + (float)(-scroll + ((actor->x) + (int)self->font_size)),
            actor->y + self->font_size + (line * (self->font_size + self->font_spacing)),
            actor->z + 0.1f,
            self->fg_r, self->fg_g, self->fg_b, // rgb
            1.0f, // a
            self->font_size,
            self->font_spacing,
            0,
            state
        );

        line += 1;

        ptr = strtok((void*)0, "\r");
    }

    glDisable(GL_SCISSOR_TEST);
}

void component_textable_draw_caret(component_textable_T* self)
{
    actor_component_T* actor_component = (actor_component_T*) self;
    actor_T* actor = (actor_T*) actor_component;

    state_T* state = get_current_state();

    if (!actor_component->focused)
        return;

    int scroll = component_textable_calculate_scroll(self);
    int y = 0;
    int x = 1;

    for (int i = 0; i < strlen(self->value); i++)
    {
        char c = self->value[i];
        
        if (i >= self->caret_position - 1)
        {
            self->caret_x = actor->x + (x * (self->font_size + self->font_spacing));
            self->caret_y = actor->y + (y * (self->font_size + self->font_spacing));

            if (self->draw_caret)
            {
                draw_positioned_2D_mesh(
                    self->caret_x - scroll,
                    actor->y + (y * (self->font_size + self->font_spacing)),
                    actor->z + 0.3f,
                    4,
                    (self->font_size * 2),
                    self->fg_r,
                    self->fg_g,
                    self->fg_b,
                    1.0f,
                    state       
                );
            }

            break;
        }

        if (c == '\r')
        {
            y += 1;
            x = 0;
        }
        else
        {
            x += 1;
        }
    }
}

/**
 * Logic that makes sure that the cursor is blinking.
 */
void component_textable_caret_blink(component_textable_T* self)
{
    struct timeval end;
    gettimeofday(&end, 0);

    float time_spent = (double)(end.tv_usec - self->timer.tv_usec)\
                       / 1000000 + (double)(end.tv_sec - self->timer.tv_sec);

    if (time_spent >= 0.5f)
    {
        if (self->draw_caret)
            self->draw_caret = 0;
        else
            self->draw_caret = 1;

        gettimeofday(&self->timer, 0);
    }
}

/**
 * This one looks for keyboard inputs every frame and performs action
 * upon them.
 */
void component_textable_handle_keyboard_input(component_textable_T* self)
{
    // first check for character input, and do nothing else here
    // if the user is trying to type something, that is why we return.
    if (KEYBOARD_STATE->character)
        return component_textable_handle_character_input(self);

    if (KEYBOARD_STATE->keys[GLFW_KEY_BACKSPACE] && !KEYBOARD_STATE->key_locks[GLFW_KEY_BACKSPACE])
    {
        if (self->caret_position > 0)
        {
            if (self->value[self->caret_position-1] == '\n')
            {
                self->caret_position -= 1;
                str_erase(&self->value, self->caret_position);
            }

            if (self->caret_position > 0)
            {
                self->caret_position -= 1;
                str_erase(&self->value, self->caret_position); 
            }
        }

        KEYBOARD_STATE->key_locks[GLFW_KEY_BACKSPACE] = 1;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_ENTER] && !KEYBOARD_STATE->key_locks[GLFW_KEY_ENTER])
    {
        char* char_str = calloc(3, sizeof(char));
        char_str[0] = '\r';
        char_str[1] = '\n';
        char_str[2] = '\0';

        self->value = realloc(self->value, (strlen(self->value) + strlen(char_str) + 1) * sizeof(char));
        insert_substring(self->value, char_str, self->caret_position + 1);
        free(char_str);

        self->caret_position += 2;

        KEYBOARD_STATE->key_locks[GLFW_KEY_ENTER] = 1;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_LEFT] && !KEYBOARD_STATE->key_locks[GLFW_KEY_LEFT])
    {
        if (self->caret_position > 0)
            self->caret_position -= 1;

        KEYBOARD_STATE->key_locks[GLFW_KEY_LEFT] = 1;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_RIGHT] && !KEYBOARD_STATE->key_locks[GLFW_KEY_RIGHT])
    {
        if (self->caret_position < strlen(self->value))
            self->caret_position += 1;

        KEYBOARD_STATE->key_locks[GLFW_KEY_RIGHT] = 1;
    }

    if (
        (KEYBOARD_STATE->keys[GLFW_KEY_UP] && !KEYBOARD_STATE->key_locks[GLFW_KEY_UP]) ||
        (KEYBOARD_STATE->keys[GLFW_KEY_DOWN] && !KEYBOARD_STATE->key_locks[GLFW_KEY_DOWN])
    )
    {
        if (KEYBOARD_STATE->keys[GLFW_KEY_UP])
        {
            while (self->value[self->caret_position] != '\r' && self->caret_position > 0)
                self->caret_position -= 1;

            while (self->value[self->caret_position] != '\n' && self->caret_position > 0)
                self->caret_position -= 1;
        
            KEYBOARD_STATE->key_locks[GLFW_KEY_UP] = 1;
        }

        if (KEYBOARD_STATE->keys[GLFW_KEY_DOWN])
        {
            while (self->value[self->caret_position] != '\n' && self->caret_position < strlen(self->value))
                self->caret_position += 1;

            while (self->value[self->caret_position] != '\r' && self->caret_position < strlen(self->value))
                self->caret_position += 1;
        
            KEYBOARD_STATE->key_locks[GLFW_KEY_DOWN] = 1;
        }
    }
}

void component_textable_handle_character_input(component_textable_T* self)
{
    char c = KEYBOARD_STATE->character;

    char* char_str = calloc(2, sizeof(char));
    char_str[0] = c;
    char_str[1] = '\0';

    self->value = realloc(self->value, (strlen(self->value) + strlen(char_str) + 1) * sizeof(char));
    insert_substring(self->value, char_str, self->caret_position + 1);
    free(char_str);

    self->caret_position += 1;
}

/**
 * Calculate the number of pixels the caret is off the width
 */
int component_textable_calculate_scroll(component_textable_T* component_textable)
{
    int scroll = 0;

    if (component_textable->value != (void*) 0)
    {
        if (strlen(component_textable->value))
        {
            int available_space = component_textable->width / (component_textable->font_size + component_textable->font_spacing);
            int chars_off = (
                component_textable->caret_x / (component_textable->font_size + component_textable->font_spacing)
            ) - available_space;

            scroll = chars_off > 0 ? chars_off * (component_textable->font_size + component_textable->font_spacing) : 0;
        }
    }

    if (scroll > 0)
        scroll += ((int)component_textable->font_size + (int)component_textable->font_spacing);

    return scroll;
}

/**
 * Returns the line number of the caret.
 */
int component_textable_get_caret_line_number(component_textable_T* component_textable)
{
    int line = 0;

    for (int i = 0; i < component_textable->caret_position; i++)
    {
        char c = component_textable->value[i];

        if (c == '\r')
            line += 1;
    }

    return line;
}

int component_textable_get_number_of_lines(component_textable_T* component_textable)
{
    int line = 0;
    
    for (int i = 0; i < strlen(component_textable->value); i++)
    {
        char c = component_textable->value[i];

        if (c == '\n')
            line += 1;
    }

    return line;
}
