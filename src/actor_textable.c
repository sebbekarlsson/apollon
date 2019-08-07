#include "include/actor_textable.h"
#include "include/string_helpers.h"
#include <string.h>
#include <coelum/draw_utils.h>
#include <coelum/current.h>
#include <coelum/constants.h>
#include <coelum/input.h>
#include <hermes/string_utils.h>


extern keyboard_state_T* KEYBOARD_STATE;


actor_textable_T* actor_textable_constructor(
    actor_textable_T* actor_textable,
    float x,
    float y,
    int width,
    int height,
    unsigned int supports_multiple_lines
)
{
    actor_focusable_T* actor_focusable = actor_focusable_constructor(
        (actor_focusable_T*) actor_textable
    );

    actor_constructor(
        (actor_T*) actor_focusable,
        x,
        y,
        0.0f,
        actor_textable_tick,
        actor_textable_draw,
        "actor_textable"
    );

    actor_textable->width = width;
    actor_textable->height = height;
    actor_textable->supports_multiple_lines = supports_multiple_lines;
    actor_textable->fg_r = 0;
    actor_textable->fg_g = 0;
    actor_textable->fg_b = 0;
    actor_textable->caret_position = 0;
    actor_textable->draw_caret = 1;
    actor_textable->font_size = 8;
    actor_textable->font_spacing = 8;
    actor_textable->value = calloc(1, sizeof(char));
    actor_textable->value[0] = '\0';
    actor_textable->caret_x = 0;
    actor_textable->caret_y = 0;

    gettimeofday(&actor_textable->timer, 0);

    return actor_textable;
}

void actor_textable_tick(actor_T* self)
{
    actor_textable_T* actor_textable = (actor_textable_T*) self;

    actor_textable_caret_blink(actor_textable);
    actor_textable_handle_keyboard_input(actor_textable);
}

void actor_textable_draw(actor_T* self)
{
    actor_textable_T* actor_textable = (actor_textable_T*) self;

    actor_textable_draw_text_value(actor_textable);
    actor_textable_draw_caret(actor_textable);
}

void actor_textable_draw_text_value(actor_textable_T* self)
{
    actor_focusable_T* actor_focusable = (actor_focusable_T*) self;
    actor_T* actor = (actor_T*) actor_focusable;

    if (self->value == (void*) 0)
        return;

    if (strlen(self->value) == 0)
        return;

    state_T* state = get_current_state();

    int scroll = actor_textable_calculate_scroll(self);

    glEnable(GL_SCISSOR_TEST);
    glScissor((int)actor->x, (int)(WINDOW_HEIGHT - actor->y - self->height), (int)self->width, (int)self->height);

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
            actor->z,
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

void actor_textable_draw_caret(actor_textable_T* self)
{
    actor_focusable_T* actor_focusable = (actor_focusable_T*) self;
    actor_T* actor = (actor_T*) actor_focusable;

    state_T* state = get_current_state();

    if (!actor_focusable->focused)
        return;

    int scroll = actor_textable_calculate_scroll(self);
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
                    actor->z,
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
void actor_textable_caret_blink(actor_textable_T* self)
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
void actor_textable_handle_keyboard_input(actor_textable_T* self)
{
    // first check for character input, and do nothing else here
    // if the user is trying to type something, that is why we return.
    if (KEYBOARD_STATE->character)
        return actor_textable_handle_character_input(self);

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
        int cx = self->caret_x;
        int i = 0;
        char c = '\0';

        while ((c = self->value[i]) != '\n' && i < strlen(self->value))
        {
            if (KEYBOARD_STATE->keys[GLFW_KEY_UP])
            {
                if (self->caret_position > 0)
                    self->caret_position -= 1;
            }
            else
            {
                if (self->caret_position < strlen(self->value))
                    self->caret_position += 1;
            }

            i+= 1;
        }

        while (self->caret_x > cx)
            self->caret_position -= 1;

        while (self->caret_x < cx-1)
            self->caret_position += 1;

        if (KEYBOARD_STATE->keys[GLFW_KEY_UP])
        {
            if (self->caret_position > 0)
                self->caret_position -= 1;
        }
        else
        {
            if (self->caret_position < strlen(self->value))
                self->caret_position += 1;
        }

        KEYBOARD_STATE->key_locks[GLFW_KEY_UP] = 1;
        KEYBOARD_STATE->key_locks[GLFW_KEY_DOWN] = 1;
    }
}

void actor_textable_handle_character_input(actor_textable_T* self)
{
    char c = KEYBOARD_STATE->character;

    char* char_str = hermes_char_to_string(c);

    self->value = realloc(self->value, (strlen(self->value) + strlen(char_str) + 1) * sizeof(char));
    insert_substring(self->value, char_str, self->caret_position + 1);
    free(char_str);

    self->caret_position += 1;
}

/**
 * Calculate the number of pixels the caret is off the width
 */
int actor_textable_calculate_scroll(actor_textable_T* actor_textable)
{
    int scroll = 0;

    if (actor_textable->value != (void*) 0)
    {
        if (strlen(actor_textable->value))
        {
            int available_space = actor_textable->width / (actor_textable->font_size + actor_textable->font_spacing);
            int chars_off = (
                actor_textable->caret_x / (actor_textable->font_size + actor_textable->font_spacing)
            ) - available_space;

            scroll = chars_off > 0 ? chars_off * (actor_textable->font_size + actor_textable->font_spacing) : 0;
        }
    }

    if (scroll > 0)
        scroll += ((int)actor_textable->font_size + (int)actor_textable->font_spacing);

    return scroll;
}

/**
 * Returns the line number of the caret.
 */
int actor_textable_get_caret_line_number(actor_textable_T* actor_textable)
{
    int line = 0;

    for (int i = 0; i < actor_textable->caret_position; i++)
    {
        char c = actor_textable->value[i];

        if (c == '\r')
            line += 1;
    }

    return line;
}

int actor_textable_get_number_of_lines(actor_textable_T* actor_textable)
{
    int line = 0;
    
    for (int i = 0; i < strlen(actor_textable->value); i++)
    {
        char c = actor_textable->value[i];

        if (c == '\n')
            line += 1;
    }

    return line;
}
