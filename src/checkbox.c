#include "include/checkbox.h"
#include <coelum/current.h>
#include <coelum/draw_utils.h>
#include <coelum/input.h>


extern keyboard_state_T* KEYBOARD_STATE;
extern texture_T* TEXTURE_CHECKBOX_SHEET;
extern const float COLOR_FG[3];
extern const float COLOR_BG_DARK_BRIGHT[3];

checkbox_T* init_checkbox(
    float x,
    float y,
    float z    
)
{
    checkbox_T* checkbox = calloc(1, sizeof(struct CHECKBOX_STRUCT));

    actor_focusable_T* actor_focusable = (actor_focusable_T*) checkbox;
    actor_focusable_constructor(actor_focusable);

    actor_T* actor = (actor_T*) actor_focusable;
    actor_constructor(actor, x, y, z, checkbox_tick, checkbox_draw, "checkbox");

    checkbox->width = 33;
    checkbox->height = 33;
    checkbox->checked = 0;

    texture_T* texture_unchecked = get_subtexture(TEXTURE_CHECKBOX_SHEET, 0, 0, 32, 32);
    texture_T* texture_checked = get_subtexture(TEXTURE_CHECKBOX_SHEET, 32, 0, 32, 32);
    dynamic_list_T* texture_list = init_dynamic_list(sizeof(struct TEXTURE_STRUCT));
    dynamic_list_append(texture_list, texture_unchecked);
    dynamic_list_append(texture_list, texture_checked);
    actor->sprite = init_sprite(texture_list, 0.1f, 25, 25);
    actor->sprite->animate = 0; 

    return checkbox;
}

void checkbox_tick(actor_T* self)
{
    checkbox_T* checkbox = (checkbox_T*) self;
    actor_focusable_T* actor_focusable = (actor_focusable_T*) self;
    unsigned int focused = actor_focusable->focused;

    if (focused)
    {
        if (KEYBOARD_STATE->keys[GLFW_KEY_ENTER] && !KEYBOARD_STATE->key_locks[GLFW_KEY_ENTER])
        {
            if (checkbox->checked)
            {
                checkbox->checked = 0;
            }
            else
            {
                checkbox->checked = 1;
            }
            
            KEYBOARD_STATE->key_locks[GLFW_KEY_ENTER] = 1;
        }
    }

    self->sprite->index = checkbox->checked;

    if (focused)
    {
        self->sprite->r = COLOR_BG_DARK_BRIGHT[0];
        self->sprite->g = COLOR_BG_DARK_BRIGHT[1];
        self->sprite->b = COLOR_BG_DARK_BRIGHT[2]; 
    }
    else
    {
        self->sprite->r = COLOR_FG[0];
        self->sprite->g = COLOR_FG[1];
        self->sprite->b = COLOR_FG[2];
    }
}

void checkbox_draw(actor_T* self)
{
}
