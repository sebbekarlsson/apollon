#include "include/component_label.h"
#include <coelum/draw_utils.h>
#include <coelum/current.h>
#include <string.h>


extern const float COLOR_FG[3];


component_label_T* init_component_label(focus_manager_T* focus_manager, float x, float y, float z, char* text)
{
    component_label_T* component_label = calloc(1, sizeof(struct COMPONENT_LABEL_STRUCT));
    actor_T* actor = (actor_T*) component_label;
    actor_constructor(actor, x, y, z, component_label_tick, component_label_draw, "component_label");
    actor_component_T* actor_component = (actor_component_T*) component_label;
    actor_component_constructor(actor_component, focus_manager, (void*)0);

    component_label->text = text;
    component_label->font_size = 6;
    component_label->font_spacing = component_label->font_size;
    component_label->r = COLOR_FG[0];
    component_label->g = COLOR_FG[1];
    component_label->b = COLOR_FG[2];
    actor->height = component_label->font_size + component_label->font_spacing;

    return component_label;
}

void component_label_tick(actor_T* self)
{
    component_label_T* component_label = (component_label_T*) self;
    self->width = strlen(component_label->text) * (component_label->font_size + component_label->font_spacing);
}

void component_label_draw(actor_T* self)
{
    component_label_T* component_label = (component_label_T*) self;
    actor_component_T* actor_component = (actor_component_T*) component_label;

    if (!actor_component->visible)
        return;

    scene_T* scene = get_current_scene();
    state_T* state = (state_T*) scene;

    /*draw_positioned_2D_mesh(
        self->x,
        self->y,
        0.0f,
        self->width,
        self->height,
        255,
        0,
        0,
        1,
        state
    );*/

    draw_text(
        component_label->text,
        self->x + ((component_label->font_size + component_label->font_spacing) / 2),
        self->y + (self->height / 2),
        self->z,
        component_label->r,
        component_label->g,
        component_label->b,
        1.0f, // a
        component_label->font_size,
        component_label->font_spacing,
        0,
        state
    );
}
