#ifndef APOLLON_SCENE_MENU_H
#define APOLLON_SCENE_MENU_H
#include "scene_base.h"


typedef struct SCENE_MENU_STRUCT {
    scene_base_T base;
} scene_menu_T;

scene_menu_T* init_scene_menu();

void scene_menu_tick(scene_T* self);

void scene_menu_draw(scene_T* self);

void scene_menu_refresh_state(scene_base_T* scene_base);
#endif
