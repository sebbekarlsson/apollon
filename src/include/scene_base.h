#ifndef APOLLON_SCENE_BASE_H
#define APOLLON_SCENE_BASE_H
#include "focus_manager.h"
#include <coelum/scene.h>


typedef struct SCENE_BASE_STRUCT
{
    scene_T base;
    focus_manager_T* focus_manager;
    void (*refresh_state)(struct SCENE_BASE_STRUCT* self);
} scene_base_T;

scene_base_T* scene_base_constructor(scene_base_T* scene_base, void (*refresh_state)(struct SCENE_BASE_STRUCT* self));

void scene_base_tick(scene_base_T* scene_base);

#define REGISTER_FOCUSABLE(scene_base, focusable)\
    dynamic_list_append(scene_base->focus_manager->focusables, focusable);\
    dynamic_list_append(((state_T*)((scene_T*)scene_base))->actors, (actor_T*)focusable);

#define REFRESH_STATE(child_scene)\
    printf("REFRESHING STATE...\n");\
    ((scene_base_T*)child_scene)->refresh_state((scene_base_T*)child_scene);

#endif
