#include "include/label.h"
#include <coelum/draw_utils.h>
#include <coelum/current.h>


label_T* init_label(float x, float y, float z, char* text)
{
    label_T* label = calloc(1, sizeof(struct LABEL_STRUCT));
    actor_T* actor = (actor_T*) label;
    actor_constructor(actor, x, y, z, label_tick, label_draw, "label");

    label->text = text;
    label->font_size = 8;
    label->font_spacing = label->font_size + 4;
    label->r = 0;
    label->g = 0;
    label->b = 0;
    label->visible = 1;

    return label;
}

void label_tick(actor_T* self)
{
}

void label_draw(actor_T* self)
{
    label_T* label = (label_T*) self;

    if (!label->visible)
        return;

    scene_T* scene = get_current_scene();
    state_T* state = (state_T*) scene;

    draw_text(
        label->text,
        self->x + ((label->font_size + label->font_spacing) / 2),
        self->y,
        0.0f,
        label->r,
        label->g,
        label->b,
        label->font_size,
        label->font_spacing,
        0,
        state
    );
}
