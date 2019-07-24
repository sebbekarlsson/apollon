#ifndef APOLLON_SCENE_SCENE_EDITOR_H
#define APOLLON_SCENE_SCENE_EDITOR_H
#include <coelum/scene.h>
#include "focus_manager.h"
#include "dropdown_list.h"
#include "button.h"
#include "input_field.h"
#include "label.h"
#include "checkbox.h"


typedef struct SCENE_SCENE_EDITOR_STRUCT
{
    scene_T base;

    label_T* label_scene;
    dropdown_list_T* dropdown_list_scene;

    label_T* label_name;
    input_field_T* input_field_name;

    label_T* label_main_scene;
    checkbox_T* checkbox_main_scene;

    button_T* button_design;
    button_T* button_save;
    button_T* button_new;
    focus_manager_T* focus_manager;
    char* scene_id; // the scene which is currenlty being modified.
} scene_scene_editor_T;

scene_scene_editor_T* init_scene_scene_editor();

void scene_scene_editor_tick(scene_T* self);

void scene_scene_editor_draw(scene_T* self);
#endif
