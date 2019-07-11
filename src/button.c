#include "include/button.h"
#include <coelum/current.h>
#include <coelum/draw_utils.h>
#include <string.h>


extern const float COLOR_BG_DARK[3];
extern const float COLOR_BG_DARK_BRIGHT[3];
extern const float COLOR_FG[3];

button_T* init_button(float x, float y, float z, char* text)
{
    button_T* button = calloc(1, sizeof(struct BUTTON_STRUCT));
    actor_focusable_T* focusable = (actor_focusable_T*) button;
    actor_focusable_constructor(focusable);
    actor_T* actor = (actor_T*) button;
    actor_constructor(actor, x, y, z, button_tick, button_draw, "button");

    button->text = text;
    button->fg_r = COLOR_FG[0];
    button->fg_g = COLOR_FG[1];
    button->fg_b = COLOR_FG[2];
    button->bg_r = COLOR_BG_DARK[0];
    button->bg_g = COLOR_BG_DARK[1];
    button->bg_b = COLOR_BG_DARK[2];
    button->font_size = 6;
    button->font_spacing = 6;
    button->width = 200;
    button->height = 32;

    return button;
}

void button_tick(actor_T* self)
{
}

void button_draw(actor_T* self)
{
    button_T* button = (button_T*) self;
    actor_focusable_T* actor_focusable = (actor_focusable_T*) button;
    unsigned int focused = actor_focusable->focused;

    scene_T* scene = get_current_scene();
    state_T* state = (state_T*) scene;

    draw_positioned_2D_mesh(
        self->x,
        self->y,
        0.0f,
        button->width,
        button->height,
        focused ? COLOR_BG_DARK_BRIGHT[0] : button->bg_r,
        focused ? COLOR_BG_DARK_BRIGHT[1] : button->bg_g,
        focused ? COLOR_BG_DARK_BRIGHT[2] : button->bg_b,
        state
    );

    draw_text(
        button->text,
        self->x + (button->width / 2) - ((strlen(button->text) * (button->font_size + button->font_spacing)) / 2),
        self->y + button->height / 2,
        0,
        focused ? 255 : button->fg_r,
        focused ? 255 : button->fg_g,
        focused ? 255 : button->fg_b,
        button->font_size,
        button->font_spacing,
        state
    );
}
