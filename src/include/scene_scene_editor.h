#ifndef APOLLON_SCENE_SCENE_EDITOR_H
#define APOLLON_SCENE_SCENE_EDITOR_H
#include <coelum/scene.h>
#include "grid.h"


typedef struct SCENE_SCENE_EDITOR_STRUCT
{
    scene_T base;
    grid_T* grid;
} scene_scene_editor_T;

scene_scene_editor_T* init_scene_scene_editor();

void scene_scene_editor_tick(scene_T* self);

void scene_scene_editor_draw(scene_T* self);
#endif
