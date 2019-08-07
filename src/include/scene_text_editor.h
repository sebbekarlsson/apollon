#ifndef APOLLON_SCENE_TEXT_EDITOR_H
#define APOLLON_SCENE_TEXT_EDITOR_H
#include <coelum/scene.h>
#include "focus_manager.h"
#include "textarea.h"


typedef struct SCENE_TEXT_EDITOR_STRUCT
{
    scene_T base;
    focus_manager_T* focus_manager;
    textarea_T* textarea;
    int line_bar_width;
} scene_text_editor_T;


scene_text_editor_T* init_scene_text_editor();

void scene_text_editor_tick(scene_T* self);

void scene_text_editor_draw(scene_T* self);
#endif
