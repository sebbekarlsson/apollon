#include "include/grid.h"
#include <coelum/theatre.h>
#include <coelum/scene_manager.h>
#include <coelum/draw_utils.h>
#include <stdlib.h>


extern theatre_T* THEATRE;

grid_T* init_grid(
    float x,
    float y,
    float z,
    float width,
    float height,
    float cell_size,
    float r,
    float g,
    float b
)
{
    grid_T* grid = calloc(1, sizeof(struct GRID_STRUCT));
    actor_T* actor = (actor_T*) grid;

    actor_constructor(actor, x, y, z, grid_tick, grid_draw, "grid");

    grid->width = width;
    grid->height = height;
    grid->cell_size = cell_size;
    grid->r = r;
    grid->g = g;
    grid->b = b;

    return grid;
}

void grid_tick(actor_T* self)
{
}

void grid_draw(actor_T* self)
{
    grid_T* grid = (grid_T*) self;

    scene_T* scene = scene_manager_get_current_scene(THEATRE->scene_manager);
    state_T* state = (state_T*) scene;

    // draw lines downwards
    for (int x = 0; x < grid->width / grid->cell_size; x++)
    {
        float cell_x = self->x + (x * grid->cell_size);

        draw_line(
            cell_x,
            self->y,
            0.0f,
            cell_x,
            self->y + grid->height,
            0.0f,
            grid->r,
            grid->g,
            grid->b,
            state
        ); 
    }

    // draw lines sideways
    for (int y = 0; y < grid->height / grid->cell_size; y++)
    {
        float cell_y = self->y + (y * grid->cell_size);

        draw_line(
            self->x,
            cell_y,
            0.0f,
            self->x + grid->width,
            cell_y,
            0.0f,
            grid->r,
            grid->g,
            grid->b,
            state
        );
    }
}
