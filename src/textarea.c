#include "include/textarea.h"
#include <coelum/current.h>
#include <coelum/draw_utils.h>
#include <coelum/constants.h>
#include <coelum/input.h>
#include <string.h>


extern keyboard_state_T* KEYBOARD_STATE;


textarea_T* init_textarea(float x, float y, float z, int width, int height)
{
    textarea_T* textarea = calloc(1, sizeof(struct TEXTAREA_STRUCT));
    actor_T* actor = (actor_T*) textarea;
    actor_constructor(actor, x, y, z, textarea_tick, textarea_draw, "textarea");
    actor_focusable_T* actor_focusable = (actor_focusable_T*) textarea;
    actor_focusable_constructor(actor_focusable);

    textarea->value = calloc(1, sizeof(char));
    textarea->value[0] = '\0';
    textarea->font_size = 8;
    textarea->font_spacing = textarea->font_size + 4;
    textarea->width = width;
    textarea->height = height;

    textarea->caret_x = 0;
    textarea->caret_y = 0;
    textarea->draw_caret = 1;

    gettimeofday(&textarea->timer, 0);

    return textarea;
}

void textarea_tick(actor_T* self)
{
    textarea_T* textarea = (textarea_T*) self;
    actor_focusable_T* actor_focusable = (actor_focusable_T*) textarea;

    if (!actor_focusable->focused)
    {
        return;
    }

    if (!strlen(textarea->value))
    {
        textarea->caret_x = 0;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_BACKSPACE] && !KEYBOARD_STATE->key_locks[GLFW_KEY_BACKSPACE])
    { 
        if (strlen(textarea->value) > 0)
        {
            int len = strlen(textarea->value); 
             
            unsigned int erased = 0;

            textarea->value[len - 1] = '\0';
            erased -= 1;

            if (textarea->value[len - 2] == '\n')
            {
                textarea->value[len - 2] = '\0';
                erased -= 1;
            }

            if (textarea->value[len - 3] == '\r')
            {
                textarea->value[len - 3] = '\0';
                erased -= 1;
            }

            printf("x:%d\n", textarea->caret_x);
            if (textarea->caret_x == 0 && textarea->caret_y > 0)
            {
                printf("y:%d\n", textarea->caret_y);
                textarea->caret_y -= 1;
            }

            char* current_line_str = textarea_get_copy_of_current_line(textarea);

            if (current_line_str != (void*) 0)
            {
                textarea->caret_x = strlen(current_line_str);
            }
            else
            {
                textarea->caret_x = 0;
            } 
        } 
        
        KEYBOARD_STATE->key_locks[GLFW_KEY_BACKSPACE] = 1;
    }
    else if (KEYBOARD_STATE->character)
    { // alright, we are typing something.
        char* char_str = calloc(2, sizeof(char));
        char_str[0] = KEYBOARD_STATE->character;
        char_str[1] = '\0';

        textarea->value = realloc(textarea->value, strlen(textarea->value) + 2);

        strcat(textarea->value, char_str);
        textarea->caret_x += 1;
    }
    
    if (KEYBOARD_STATE->keys[GLFW_KEY_ENTER] && !KEYBOARD_STATE->key_locks[GLFW_KEY_ENTER])
    {
        char* char_str = calloc(3, sizeof(char));
        char_str[0] = '\r';
        char_str[1] = '\n';
        char_str[2] = '\0';

        textarea->value = realloc(textarea->value, strlen(textarea->value) + 3);

        strcat(textarea->value, char_str);

        textarea->caret_x = 0;
        textarea->caret_y += 1;

        KEYBOARD_STATE->key_locks[GLFW_KEY_ENTER] = 1;
    }

    struct timeval end;
    gettimeofday(&end, 0);

    float time_spent = (double)(end.tv_usec - textarea->timer.tv_usec) / 1000000 + (double)(end.tv_sec - textarea->timer.tv_sec);

    if (time_spent >= 0.5f)
    {
        if (textarea->draw_caret)
        {
            textarea->draw_caret = 0;
        }
        else
        {
            textarea->draw_caret = 1;

        }

        gettimeofday(&textarea->timer, 0);
    }
}

void textarea_draw(actor_T* self)
{
    textarea_T* textarea = (textarea_T*) self;
    actor_focusable_T* actor_focusable = (actor_focusable_T*) textarea;
    state_T* state = (state_T*) get_current_scene();

    draw_positioned_2D_mesh(
        self->x,
        self->y,
        self->z,
        textarea->width,
        textarea->height,
        0, 0, 0, // rgb
        1.0f, // a
        state 
    );

    float scroll = 0;

    if (textarea->value)
    {
        if (strlen(textarea->value))
        {
            int available_space = textarea->width / (textarea->font_size + textarea->font_spacing);
            int chars_off = /*strlen(textarea->value)*/ textarea->caret_x - available_space;

            scroll = chars_off > 0 ? chars_off * (textarea->font_size + textarea->font_spacing) : 0;

            glEnable(GL_SCISSOR_TEST);
            glScissor((int)self->x, (int)(WINDOW_HEIGHT - self->y - textarea->height), (int)textarea->width, (int)textarea->height);

            int line_nr = 0;

            char text[1024];
            strcpy(text, textarea->value);

            char* ptr = strtok(text, "\n");

            while (ptr)
            {
                if (ptr[strlen(ptr) - 1] == '\r')
                    ptr[strlen(ptr) - 1] = '\0';

                draw_text(
                    ptr,
                    self->x + (((textarea->font_size + textarea->font_spacing) / 2) - scroll),
                    self->y + (textarea->font_size) + (line_nr * (textarea->font_size + textarea->font_spacing)),
                    self->z,
                    255, 255, 255, // rgb
                    1.0f, // a
                    textarea->font_size,
                    textarea->font_spacing,
                    0,
                    state
                );

                line_nr += 1;
                ptr = strtok((void*) 0, "\n");
            }

            glDisable(GL_SCISSOR_TEST);
        }
    }

    if (textarea->draw_caret && actor_focusable->focused)
    {
        draw_positioned_2D_mesh(
            self->x + ((textarea->font_size + textarea->font_spacing) * (float) textarea->caret_x - ((textarea->font_size + textarea->font_spacing) / 2)) + ((textarea->font_size + textarea->font_spacing) / 2) - scroll,
            self->y + (textarea->caret_y * (textarea->font_size + textarea->font_spacing)),
            self->z,
            4,
            (textarea->font_size * 2),
            255, 255, 255, // rgb
            1.0f,
            state       
        );
    } 
}

char* textarea_get_copy_of_current_line(textarea_T* textarea)
{
    char* line = 0;
    char val[1024];
    strcpy(val, textarea->value);

    char* ptr = strtok(val, "\n");
    int line_nr = 0;

    while (ptr)
    {
        if (line_nr == textarea->caret_y)
        {
            line = calloc(strlen(ptr) + 1, sizeof(char));
            strcpy(line, ptr);

            return line;
        }

        ptr = strtok((void*) 0, "\n");
        line_nr += 1;
    }
    
    return (void*) 0;
}
