#include "include/component_pane.h"
#include <coelum/draw_utils.h>


component_pane_T* init_component_pane(state_T* state, int x, int y, int width, int height)
{
    component_pane_T* component_pane = calloc(1, sizeof(struct COMPONENT_PANE_STRUCT));
    scene_T* scene = (scene_T*) component_pane;

    scene_constructor(scene, component_pane_tick, component_pane_draw, 2);

    component_pane->component_index = 0;
    component_pane->x = x;
    component_pane->y = y;
    component_pane->width = width;
    component_pane->height = height;
    component_pane->state = state;
    component_pane->components = init_dynamic_list(sizeof(struct COMPONENT_STRUCT*));

    return component_pane;
}

void component_pane_tick(scene_T* self)
{
    component_pane_adjust((component_pane_T*) self);
}

void component_pane_draw(scene_T* self)
{
    state_T* state = (state_T*) self;
    component_pane_T* component_pane = (component_pane_T*) self;

    // black
    draw_positioned_2D_mesh(
        component_pane->x,
        component_pane->y,
        0.0f,
        component_pane->width,
        component_pane->height,
        0.0f,
        0.0f,
        0.0f,
        1.0f,
        state
    );

    int padding = 4;

    // white
    draw_positioned_2D_mesh(
        component_pane->x + padding,
        component_pane->y + padding,
        0.0f,
        component_pane->width - (padding*2),
        component_pane->height - (padding*2),
        255.0f,
        255.0f,
        255.0f,
        1.0f,
        state
    );
}

actor_component_T* component_pane_add_component(component_pane_T* component_pane, actor_component_T* actor_component)
{
    state_T* state = (state_T*)((scene_T*) component_pane);

    dynamic_list_append(state->actors, (actor_T*) actor_component);
    dynamic_list_append(component_pane->components, actor_component);

    return actor_component;
}

void component_pane_adjust(component_pane_T* component_pane)
{
    int padding = 4;
    int x_padding = 0;

    for (int i = 0; i < component_pane->components->size; i++)
    {
        actor_component_T* ac = (actor_component_T*) component_pane->components->items[i];
        actor_component_T* acp =
            (actor_component_T*) component_pane->components->items[
            i > 0 ? i-1 : 0
        ];
        actor_T* a = (actor_T*) ac;
        actor_T* ap = (actor_T*) acp;

        if (i != 0)
            x_padding += ap->width;

        if (a->y < (component_pane->y + padding))
        {
            int diff = ((component_pane->y + padding) - (a->y + a->height));
            a->y -= diff;
        }

        if (a->x < ((component_pane->x + padding) + x_padding))
        {
            int diff = (((component_pane->x + padding)) - (a->x));
            a->x += (diff + x_padding);
        }
    }
}
