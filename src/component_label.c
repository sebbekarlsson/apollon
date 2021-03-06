#include "include/component_label.h"
#include "include/string_utils.h"
#include <coelum/draw_utils.h>
#include <coelum/current.h>
#include <string.h>


component_label_T* init_component_label(focus_manager_T* focus_manager, float x, float y, float z, char* text, unsigned int r, unsigned int g, unsigned int b)
{
    component_label_T* component_label = calloc(1, sizeof(struct COMPONENT_LABEL_STRUCT));
    actor_T* actor = (actor_T*) component_label;
    actor_constructor(actor, x, y, z, component_label_tick, component_label_draw, "component_label");
    actor_component_T* actor_component = (actor_component_T*) component_label;
    actor_component_constructor(actor_component, focus_manager, (void*)0);
    
    actor->height = component_label->font_size + component_label->font_spacing;

    component_label->text = string_copy(text);
    component_label->font_size = 6;
    component_label->font_spacing = component_label->font_size;
    component_label->r = r;
    component_label->g = g;
    component_label->b = b;

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

    state_T* state = get_current_state();

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
