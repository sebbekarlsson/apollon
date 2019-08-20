#ifndef APOLLON_SCENE_SCENE_EDITOR_H
#define APOLLON_SCENE_SCENE_EDITOR_H
#include "scene_base.h"
#include "component_dropdown_list.h"
#include "component_button.h"
#include "component_input_field.h"
#include "component_label.h"
#include "component_checkbox.h"


typedef struct SCENE_SCENE_EDITOR_STRUCT
{
    scene_base_T base;

    component_label_T* component_label_scene;
    component_dropdown_list_T* component_dropdown_list_scene;

    component_label_T* component_label_number_of_actors;

    component_label_T* component_label_name;
    component_input_field_T* component_input_field_name;

    component_label_T* component_label_main_scene;
    component_checkbox_T* component_checkbox_main_scene;

    component_button_T* component_button_design;
    component_button_T* component_button_save;
    component_button_T* component_button_new;
    component_button_T* component_button_delete;
    char* scene_id; // the scene which is currenlty being modified.
} scene_scene_editor_T;

scene_scene_editor_T* init_scene_scene_editor();

void scene_scene_editor_tick(scene_T* self);

void scene_scene_editor_draw(scene_T* self);

void scene_scene_editor_refresh_state(scene_base_T* scene_base);
#endif
