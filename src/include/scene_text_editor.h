#ifndef APOLLON_SCENE_TEXT_EDITOR_H
#define APOLLON_SCENE_TEXT_EDITOR_H
#include <coelum/scene.h>
#include "focus_manager.h"
#include "component_textarea.h"


typedef struct SCENE_TEXT_EDITOR_STRUCT
{
    scene_T base;
    focus_manager_T* focus_manager;
    component_textarea_T* component_textarea;
} scene_text_editor_T;


scene_text_editor_T* init_scene_text_editor();

void scene_text_editor_tick(scene_T* self);
#endif
