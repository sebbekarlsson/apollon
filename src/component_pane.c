#include "include/component_pane.h"
#include <coelum/draw_utils.h>
#include <coelum/constants.h>


component_pane_T* init_component_pane(state_T* state, focus_manager_T* focus_manager, int x, int y, int width, int height)
{
    component_pane_T* component_pane = calloc(1, sizeof(struct COMPONENT_PANE_STRUCT));
    scene_T* scene = (scene_T*) component_pane;

    scene_constructor(scene, component_pane_tick, component_pane_draw, 2);

    component_pane->component_index = 0;
    component_pane->x = x;
    component_pane->y = y;
    component_pane->z = 0.0f;
    component_pane->child_margin_top = 0;
    component_pane->width = width;
    component_pane->height = height;
    component_pane->state = state;
    component_pane->min_height = 0;
    component_pane->focus_manager = focus_manager;
    component_pane->components = init_dynamic_list(sizeof(struct COMPONENT_STRUCT*));
    component_pane->rows = init_dynamic_list(sizeof(struct COMPONENT_PANE_STRUCT*));
    component_pane->cols = init_dynamic_list(sizeof(struct COMPONENT_PANE_STRUCT*));
    component_pane->bg_visible = 1;

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

    if (component_pane->bg_visible)
    {
        int padding = 4;

        // black
        draw_positioned_2D_mesh(
            component_pane->x,
            component_pane->y,
            component_pane->z,
            component_pane->width,
            component_pane->height,
            0.0f,
            0.0f,
            0.0f,
            1.0f,
            state
        );


        // white
        draw_positioned_2D_mesh(
            component_pane->x + padding,
            component_pane->y + padding,
            component_pane->z + 0.1f,
            component_pane->width - (padding*2),
            component_pane->height - (padding*2),
            255.0f,
            255.0f,
            255.0f,
            1.0f,
            state
        );
    }

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
    dynamic_list_append(component_pane->focus_manager->components, actor_component);

    return actor_component;
}

static int compare( const void* a, const void* b)
{
     int int_a = * ( (int*) a );
     int int_b = * ( (int*) b );

     if ( int_a == int_b ) return 0;
     else if ( int_a < int_b ) return -1;
     else return 1;
}

void component_pane_adjust(component_pane_T* component_pane)
{
    int padding = 4;
    int x = 0;
    int y = 0;
    int col_width = component_pane->width / (int)(component_pane->cols->size > 0 ? component_pane->cols->size : 1);
    int row_height = component_pane->height / (int)(component_pane->rows->size > 0 ? component_pane->rows->size : 1);

    /** ==== adjust position of child components === **/

    if (component_pane->centered)
    {
        int final_height = 0;

        // calculating the final height
        for (int i = 0; i < component_pane->components->size; i++)
        {
            actor_component_T* ac = (actor_component_T*) component_pane->components->items[i];
            actor_T* a = (actor_T*) ac;
            actor_component_T* acp =
                (actor_component_T*) component_pane->components->items[
                i > 0 ? i-1 : 0
            ];
            actor_T* ap = (actor_T*) acp;
            a->z = component_pane->z + 0.2f;

            final_height += a->height + ac->margin_y + component_pane->child_margin_top;
        }

        // using the calculated final height to center vertically
        for (int i = 0; i < component_pane->components->size; i++)
        {
            actor_component_T* ac = (actor_component_T*) component_pane->components->items[i];
            actor_component_T* acp =
                (actor_component_T*) component_pane->components->items[
                i > 0 ? i-1 : 0
            ];
            actor_T* a = (actor_T*) ac;
            actor_T* ap = (actor_T*) acp;

            a->x = component_pane->x + (component_pane->width / 2) - a->width / 2;

            if (i == 0)
            {
                a->y = component_pane->y + ((component_pane->height / 2) + (i * (a->height + ac->margin_y))) - final_height / 2;
            }
            else
            {
                a->y = ap->y;
                a->y += ap->height + ac->margin_y + component_pane->child_margin_top;
            }
        }
    }
    else
    {
        int* heights = (void*)0;
        size_t heights_size = 0;

        for (int i = 0; i < component_pane->components->size; i++)
        {
            actor_component_T* ac = (actor_component_T*) component_pane->components->items[i];
            actor_component_T* acp =
                (actor_component_T*) component_pane->components->items[
                i > 0 ? i-1 : 0
            ];
            actor_T* a = (actor_T*) ac;
            actor_T* ap = (actor_T*) acp;
            a->z = component_pane->z + 0.2f;

            heights_size += 1;
            if (heights == (void*)0)
            {
                heights = calloc(heights_size, sizeof(int));
            }
            else
            {
                heights = realloc(heights, heights_size * sizeof(int));
            }
            heights[heights_size-1] = ap->height;

            qsort(heights, heights_size, sizeof(int), compare);
            int highest = heights_size ? heights[heights_size-1] : 0;

            if (i != 0)
            {
                x = ap->x + ap->width;

                if (x > component_pane->x + component_pane->width)
                {
                    y += highest + acp->margin_y + component_pane->child_margin_top;
                    x = 0;
                }
            }

            if (y < (component_pane->y + padding))
                y -= (y - (component_pane->y + padding));

            if (x < (component_pane->x + padding))
                x -= (x - (component_pane->x + padding));

            a->x = x;
            a->y = y;
        }

        if (heights && heights_size)
        {
            free(heights);
            heights_size = 0;
        }
    } 
    
    /** ==== adjust position of child columns === **/

    x = 0;
    y = 0;

    for (int i = 0; i < component_pane->cols->size; i++)
    {
        component_pane_T* child_pane = (component_pane_T*) component_pane->cols->items[i];
        child_pane->z = component_pane->z + 0.2f;

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

    row_height = WINDOW_HEIGHT;

    for (int i = 0; i < component_pane->rows->size; i++)
    {
        component_pane_T* child_pane = (component_pane_T*) component_pane->rows->items[i];
        child_pane->z = component_pane->z + 0.2f;

        actor_component_T* acp =
            (actor_component_T*) component_pane->rows->items[
            i > 0 ? i-1 : 0
        ];

        actor_T* ap = (actor_T*) acp;

        child_pane->height = child_pane->min_height != 0 ? child_pane->min_height : row_height;
        child_pane->height = child_pane->height > component_pane->height ? component_pane->height : child_pane->height;
        child_pane->width = component_pane->width; 

        child_pane->y = (component_pane->y + y);
        child_pane->x = component_pane->x;

        row_height -= child_pane->height;
        y += child_pane->height;
    }
}
