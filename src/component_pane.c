#include "include/component_pane.h"
#include <coelum/draw_utils.h>
#include <coelum/constants.h>


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
    component_pane->rows = init_dynamic_list(sizeof(struct COMPONENT_PANE_STRUCT*));
    component_pane->cols = init_dynamic_list(sizeof(struct COMPONENT_PANE_STRUCT*));

    return component_pane;
}

void component_pane_tick(scene_T* self)
{
    component_pane_T* component_pane = (component_pane_T*) self;
    
    component_pane->width = component_pane->width < 0 ? 16 : component_pane->width;

    component_pane_adjust(component_pane);

    for (int i = 0; i < component_pane->rows->size; i++)
    {
        component_pane_T* child_pane = (component_pane_T*) component_pane->rows->items[i];
        ((scene_T*)child_pane)->tick((scene_T*)child_pane);
        state_tick((state_T*)child_pane);
    }

    for (int i = 0; i < component_pane->cols->size; i++)
    {
        component_pane_T* child_pane = (component_pane_T*) component_pane->cols->items[i];
        ((scene_T*)child_pane)->tick((scene_T*)child_pane);
        state_tick((state_T*)child_pane);
    }
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

    for (int i = 0; i < component_pane->rows->size; i++)
    {
        component_pane_T* child_pane = (component_pane_T*) component_pane->rows->items[i];
        glEnable(GL_SCISSOR_TEST);
        glScissor((int)child_pane->x, (int)(WINDOW_HEIGHT - child_pane->y - child_pane->height), (int)child_pane->width, (int)child_pane->height); 
    
        ((scene_T*)child_pane)->draw((scene_T*)child_pane);
        glDisable(GL_SCISSOR_TEST);
    }

    
    for (int i = 0; i < component_pane->cols->size; i++)
    {
        component_pane_T* child_pane = (component_pane_T*) component_pane->cols->items[i];
        glEnable(GL_SCISSOR_TEST);
        glScissor((int)child_pane->x, (int)(WINDOW_HEIGHT - child_pane->y - child_pane->height), (int)child_pane->width, (int)child_pane->height); 
    
        ((scene_T*)child_pane)->draw((scene_T*)child_pane);
        glDisable(GL_SCISSOR_TEST);
    }

    for (int i = 0; i < component_pane->rows->size; i++)
    {
        component_pane_T* child_pane = (component_pane_T*) component_pane->rows->items[i];
        glEnable(GL_SCISSOR_TEST);
        glScissor((int)child_pane->x, (int)(WINDOW_HEIGHT - child_pane->y - child_pane->height), (int)child_pane->width, (int)child_pane->height); 
    
        state_draw((state_T*)((scene_T*)child_pane));
        glDisable(GL_SCISSOR_TEST);
    }

    
    for (int i = 0; i < component_pane->cols->size; i++)
    {
        component_pane_T* child_pane = (component_pane_T*) component_pane->cols->items[i];
        glEnable(GL_SCISSOR_TEST);
        glScissor((int)child_pane->x, (int)(WINDOW_HEIGHT - child_pane->y - child_pane->height), (int)child_pane->width, (int)child_pane->height); 
    
        state_draw((state_T*)((scene_T*)child_pane));
        glDisable(GL_SCISSOR_TEST);
    }
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
    int x = 0;
    int y = 0;
    int col_width = component_pane->width / (int)(component_pane->cols->size > 0 ? component_pane->cols->size : 1);
    int row_height = component_pane->height / (int)(component_pane->rows->size > 0 ? component_pane->rows->size : 1);

    /** ==== adjust position of child components === **/

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
        {
            if (ap->x + ap->width*2 < component_pane->width)
            {
                x += ap->width;
            }
            else
            {
                x = 0;
                y += ap->height;
            }
        }

        if (y < (component_pane->y + padding))
            y -= (y - (component_pane->y + padding));

        if (x < (component_pane->x + padding))
            x -= (x - (component_pane->x + padding));

        a->x = x;
        a->y = y;
    }
    
    /** ==== adjust position of child columns === **/

    x = 0;
    y = 0;

    for (int i = 0; i < component_pane->cols->size; i++)
    {
        component_pane_T* child_pane = (component_pane_T*) component_pane->cols->items[i];

        actor_component_T* acp =
            (actor_component_T*) component_pane->cols->items[
            i > 0 ? i-1 : 0
        ];

        actor_T* ap = (actor_T*) acp;

        child_pane->width = col_width;
        child_pane->width = child_pane->width > component_pane->width ? component_pane->width : child_pane->width;
        child_pane->height = component_pane->height;

        if (ap->x + child_pane->width < component_pane->width && i != 0)
            x += col_width;

        child_pane->x = component_pane->x + x;
        child_pane->y = component_pane->y;
    }

    /** ==== adjust position of child rows === **/

    x = 0;
    y = 0;

    for (int i = 0; i < component_pane->rows->size; i++)
    {
        component_pane_T* child_pane = (component_pane_T*) component_pane->rows->items[i];

        actor_component_T* acp =
            (actor_component_T*) component_pane->rows->items[
            i > 0 ? i-1 : 0
        ];

        actor_T* ap = (actor_T*) acp;

        child_pane->height = row_height;
        child_pane->height = child_pane->height > component_pane->height ? component_pane->height : child_pane->height;
        child_pane->width = component_pane->width;

        if (ap->y + ap->height*2 < component_pane->height && i != 0)
            y += row_height;

        child_pane->y = component_pane->y;
        child_pane->x = component_pane->x;
    }
}
