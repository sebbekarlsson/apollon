#ifndef APOLLON_GRID_H
#define APOLLON_GRID_H
#include <coelum/actor.h>


typedef struct GRID_STRUCT
{
    actor_T base;
    float width;
    float height;
    float cell_size;
    float r;
    float g;
    float b;
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
    float b
);

void grid_tick(actor_T* self);
void grid_draw(actor_T* self);
#endif
