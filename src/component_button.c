#include "include/component_button.h"
#include <coelum/current.h>
#include <coelum/draw_utils.h>
#include <coelum/input.h>
#include <string.h>


extern const float COLOR_BG_DARK[3];
extern const float COLOR_BG_DARK_BRIGHT[3];
extern const float COLOR_FG[3];
extern const float COLOR_RED[3];

extern keyboard_state_T* KEYBOARD_STATE;
extern mouse_state_T* MOUSE_STATE;

component_button_T* init_component_button(focus_manager_T* focus_manager, float x, float y, float z, char* text, void (*press)())
{
    component_button_T* component_button = calloc(1, sizeof(struct COMPONENT_BUTTON_STRUCT));
    actor_component_T* component = (actor_component_T*) component_button;
    actor_component_constructor(component, focus_manager, press);
    actor_T* actor = (actor_T*) component_button;
    actor_constructor(actor, x, y, z, component_button_tick, component_button_draw, "component_button");

    component_button->text = text;
    component_button->fg_r = COLOR_FG[0];
    component_button->fg_g = COLOR_FG[1];
    component_button->fg_b = COLOR_FG[2];
    component_button->bg_r = COLOR_BG_DARK[0];
    component_button->bg_g = COLOR_BG_DARK[1];
    component_button->bg_b = COLOR_BG_DARK[2];
    component_button->alpha = 1.0f;
    component_button->font_size = 6;
    component_button->font_spacing = 6;
    actor->width = 200;
    actor->height = 32;
    component_button->press = press;
    component_button->disabled = 0;

    return component_button;
}

void component_button_tick(actor_T* self)
{
    component_button_T* component_button = (component_button_T*) self;
    actor_component_T* actor_component = (actor_component_T*) component_button;
    unsigned int focused = actor_component->focused;

    actor_component_tick(actor_component); 

    if ((focused || actor_component->hovered) && !component_button->disabled)
    {
        if (KEYBOARD_STATE->keys[GLFW_KEY_ENTER] && !KEYBOARD_STATE->key_locks[GLFW_KEY_ENTER])
        {
            component_button->press();
            KEYBOARD_STATE->key_locks[GLFW_KEY_ENTER] = 1;
        }
    }

    component_button->alpha = component_button->disabled ? 0.6f : 1.0f;
}

void component_button_draw(actor_T* self)
{
    component_button_T* component_button = (component_button_T*) self;
    actor_component_T* actor_component = (actor_component_T*) component_button;
    unsigned int focused = actor_component->focused;
    unsigned int hovered = actor_component->hovered;
    unsigned int visible = actor_component->visible;
    unsigned int disabled = component_button->disabled;

    if (!visible)
        return;

    scene_T* scene = get_current_scene();
    state_T* state = (state_T*) scene;

    draw_positioned_2D_mesh(
        self->x,
        self->y,
        self->z,
        self->width,
        self->height,
        disabled ? COLOR_RED[0] : ((focused || hovered) ? COLOR_BG_DARK_BRIGHT[0] : component_button->bg_r),
        disabled ? COLOR_RED[1] : ((focused || hovered) ? COLOR_BG_DARK_BRIGHT[1] : component_button->bg_g),
        disabled ? COLOR_RED[2] : ((focused || hovered) ? COLOR_BG_DARK_BRIGHT[2] : component_button->bg_b),
        component_button->alpha,
        state
    );

    // top gloss
    draw_positioned_2D_mesh(
        self->x,
        self->y,
        self->z + 0.1f,
        self->width,
        4,
        255,
        255,
        255,
        0.4,
        state
    );

    // left gloss
    draw_positioned_2D_mesh(
        self->x,
        self->y,
        self->z + 0.1f,
        4,
        self->height,
        255,
        255,
        255,
        0.6,
        state
    );

    // bottom shadow 
    draw_positioned_2D_mesh(
        self->x,
        self->y + self->height - 4,
        self->z + 0.1f,
        self->width,
        4,
        0,
        0,
        0,
        component_button->alpha - 0.6f,
        state
    );

    // right shadow 
    draw_positioned_2D_mesh(
        self->x + self->width - 4,
        self->y,
        self->z + 0.1f,
        4,
        self->height,
        0,
        0,
        0,
        component_button->alpha - 0.6f,
        state
    );

    draw_text(
        component_button->text,
        self->x + (self->width / 2) - ((strlen(component_button->text) * (component_button->font_size + component_button->font_spacing)) / 2),
        self->y + self->height / 2,
        self->z + 0.1f,
        focused || hovered ? 255 : component_button->fg_r,
        focused || hovered ? 255 : component_button->fg_g,
        focused || hovered ? 255 : component_button->fg_b,
        1.0f, // a
        component_button->font_size,
        component_button->font_spacing,
        0,
        state
    );
}

void component_button_free(component_button_T* component_button)
{
//    free(component_button->text);

    actor_T* actor = (actor_T*) component_button;

    actor_free(actor);

    free(component_button);
}
