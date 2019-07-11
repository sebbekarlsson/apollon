#ifndef APOLLON_SCENE_ACTOR_EDITOR_H
#define APOLLON_SCENE_ACTOR_EDITOR_H
#include <coelum/scene.h>
#include <coelum/dynamic_list.h>
#include "input_field.h"
#include "dropdown_list.h"
#include "focus_manager.h"


typedef struct SCENE_ACTOR_EDITOR_STRUCT
{
    scene_T base;
    input_field_T* input_field_type_name;
    input_field_T* input_field_tick_script;
    dropdown_list_T* dropdown_list_sprites;
    focus_manager_T* focus_manager;
} scene_actor_editor_T;

scene_actor_editor_T* init_scene_actor_editor();

void scene_actor_editor_tick(scene_T* self);

void scene_actor_editor_draw(scene_T* self);
#endif
