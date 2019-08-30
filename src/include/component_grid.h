#ifndef APOLLON_COMPONENT_GRID_H
#define APOLLON_COMPONENT_GRID_H
#include "actor_component.h"
#include "focus_manager.h"
#include <coelum/textures.h>
#include <coelum/sprite.h>


typedef struct COMPONENT_GRID_CELL_STRUCT
{
    float r;
    float g;
    float b;
    float a;
    int selected;
    sprite_T* sprite;
} component_grid_cell_T;

component_grid_cell_T* init_component_grid_cell();

typedef struct COMPONENT_GRID_STRUCT
{
    actor_component_T base;
    float width;
    float height;
    float cell_size;
    float r;
    float g;
    float b;
    int cursor_x;
    int cursor_y;
    component_grid_cell_T*** cells;
    struct COMPONENT_GRID_STRUCT* onion;
} component_grid_T;

component_grid_T* init_component_grid(
    focus_manager_T* focus_manager,
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

void component_grid_tick(actor_T* self);
void component_grid_draw(actor_T* self);

int component_grid_create_image(component_grid_T* component_grid, const char* filename);

texture_T* component_grid_create_texture(component_grid_T* component_grid);

void component_grid_copy(component_grid_T* source_component_grid, component_grid_T* target_component_grid);

void component_grid_unselect_all_cells(component_grid_T* component_grid);

void component_grid_clean(component_grid_T* component_grid);

void component_grid_free(component_grid_T* component_grid);
#endif
