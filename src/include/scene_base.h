#ifndef APOLLON_SCENE_BASE_H
#define APOLLON_SCENE_BASE_H
#include "focus_manager.h"
#include "actor_focusable.h"
#include "component_pane.h"
#include <coelum/scene.h>


typedef struct SCENE_BASE_STRUCT
{
    scene_T base;
    focus_manager_T* focus_manager;
    void (*refresh_state)(struct SCENE_BASE_STRUCT* self);
    char* title;
    component_pane_T* component_pane;
} scene_base_T;

scene_base_T* scene_base_constructor(scene_base_T* scene_base, void (*refresh_state)(struct SCENE_BASE_STRUCT* self), const char* title);

void scene_base_tick(scene_base_T* scene_base);

void scene_base_draw(scene_base_T* scene_base);

void scene_base_draw_title_bar(scene_base_T* scene_base);

actor_focusable_T* scene_base_register_focusable(scene_base_T* scene_base, actor_focusable_T* focusable);

#define REFRESH_STATE(child_scene)\
    printf("REFRESHING STATE...\n");\
    ((scene_base_T*)child_scene)->refresh_state((scene_base_T*)child_scene);

#endif
