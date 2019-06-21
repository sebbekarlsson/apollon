#ifndef APOLLON_SCENE_MENU_H
#define APOLLON_SCENE_MENU_H
#include <coelum/scene.h>
#include "select_list.h"


typedef struct SCENE_MENU_STRUCT {
    scene_T base;
    select_list_T* select_list;
} scene_menu_T;

scene_menu_T* init_scene_menu();

void scene_menu_tick(scene_T* self);

void scene_menu_draw(scene_T* self);
#endif
