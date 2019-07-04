#ifndef APOLLON_GRID_H
#define APOLLON_GRID_H
#include <coelum/actor.h>

typedef struct GRID_CELL_STRUCT
{
    float r;
    float g;
    float b;
} grid_cell_T;

grid_cell_T* init_grid_cell();

typedef struct GRID_STRUCT
{
    actor_T base;
    float width;
    float height;
    float cell_size;
    float r;
    float g;
    float b;
    int cursor_x;
    int cursor_y;
    grid_cell_T*** cells;
    unsigned int focused;
} grid_T;

grid_T* init_grid(
    float x,
    float y,
    float z,
    float width,
    float height,
    float cell_size,
    float r,
    float g,
    float b,
    char* type_name
);

void grid_tick(actor_T* self);
void grid_draw(actor_T* self);

int grid_create_image(grid_T* grid, const char* filename);

void grid_copy(grid_T* source_grid, grid_T* target_grid);

void grid_free(grid_T* grid);
#endif
