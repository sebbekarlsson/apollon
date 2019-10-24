#include "include/component_checkbox.h"
#include <coelum/current.h>
#include <coelum/draw_utils.h>
#include <coelum/input.h>


extern keyboard_state_T* KEYBOARD_STATE;
extern texture_T* TEXTURE_CHECKBOX_SHEET;
extern const float COLOR_FG[3];
extern const float COLOR_BG_DARK_BRIGHT[3];


static void checkbox_click(actor_T* self)
{
    component_checkbox_T* component_checkbox = (component_checkbox_T*) self;

    if (component_checkbox->checked)
        component_checkbox->checked = 0;
    else
        component_checkbox->checked = 1;
}

component_checkbox_T* init_component_checkbox(
    focus_manager_T* focus_manager,
    float x,
    float y,
    float z    
)
{
    component_checkbox_T* component_checkbox = calloc(1, sizeof(struct COMPONENT_CHECKBOX_STRUCT));

    actor_component_T* actor_component = (actor_component_T*) component_checkbox;
    actor_component_constructor(actor_component, focus_manager, checkbox_click);

    actor_T* actor = (actor_T*) actor_component;
    actor_constructor(actor, x, y, z, component_checkbox_tick, component_checkbox_draw, "component_checkbox");

    texture_T* texture_unchecked = get_subtexture(TEXTURE_CHECKBOX_SHEET, 0, 0, 32, 32);
    texture_T* texture_checked = get_subtexture(TEXTURE_CHECKBOX_SHEET, 32, 0, 32, 32);
    dynamic_list_T* texture_list = init_dynamic_list(sizeof(struct TEXTURE_STRUCT));
    dynamic_list_append(texture_list, texture_unchecked);
    dynamic_list_append(texture_list, texture_checked);
    actor->sprite = init_sprite(texture_list, 0.1f, 25, 25);
    actor->sprite->animate = 0; 
    actor->width = 33;
    actor->height = 33;

    component_checkbox->checked = 0; 

    return component_checkbox;
}

void component_checkbox_tick(actor_T* self)
{
    component_checkbox_T* component_checkbox = (component_checkbox_T*) self;
    actor_component_T* actor_component = (actor_component_T*) self;
    unsigned int focused = actor_component->focused;

    actor_component_tick(actor_component);

    if (focused)
    {
        if (KEYBOARD_STATE->keys[GLFW_KEY_ENTER] && !KEYBOARD_STATE->key_locks[GLFW_KEY_ENTER])
        {
            if (component_checkbox->checked)
                component_checkbox->checked = 0;
            else
                component_checkbox->checked = 1;
            
            KEYBOARD_STATE->key_locks[GLFW_KEY_ENTER] = 1;
        }
    }

    self->sprite->index = component_checkbox->checked;

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

void component_checkbox_draw(actor_T* self) {}
