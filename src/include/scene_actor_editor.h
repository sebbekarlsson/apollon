#ifndef APOLLON_SCENE_ACTOR_EDITOR_H
#define APOLLON_SCENE_ACTOR_EDITOR_H
#include <coelum/scene.h>
#include "input_field.h"


typedef struct SCENE_ACTOR_EDITOR_STRUCT
{
    scene_T base;
    input_field_T* test_input;
} scene_actor_editor_T;

scene_actor_editor_T* init_scene_actor_editor();

void scene_actor_editor_tick(scene_T* self);

void scene_actor_editor_draw(scene_T* self);
#endif
