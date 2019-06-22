#ifndef APOLLON_SCENE_SPRITE_EDITOR_H
#define APOLLON_SCENE_SPRITE_EDITOR_H
#include <coelum/scene.h>
#include <coelum/dynamic_list.h>
#include "grid.h"


typedef struct SCENE_SPRITE_EDITOR_STRUCT {
    scene_T base;
    grid_T* grid;
    grid_T* grid_color_selector;
    grid_T* grid_color_mixer;
    dynamic_list_T* focusables;
    unsigned int focus_index;
    float r;
    float g;
    float b;
} scene_sprite_editor_T;

scene_sprite_editor_T* init_scene_sprite_editor();

void scene_sprite_editor_tick(scene_T* self);

void scene_sprite_editor_draw(scene_T* self);
#endif
