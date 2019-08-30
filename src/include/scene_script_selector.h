#ifndef APOLLON_SCENE_SCRIPT_SELECTOR_H
#define APOLLON_SCENE_SCRIPT_SELECTOR_H
#include "scene_base.h"
#include "component_dropdown_list.h"
#include "component_input_field.h"
#include "component_button.h"
#include "component_label.h"


typedef struct SCENE_SCRIPT_SELECTOR_STRUCT
{
    scene_base_T base;
    char* script_id;
    database_script_T* current_database_script;

    /** LEFT **/
    component_label_T* component_label_script;
    component_dropdown_list_T* component_dropdown_list_script;

    /** RIGHT **/
    component_label_T* component_label_name;
    component_input_field_T* component_input_field_name;
    component_button_T* component_button_save;
    component_button_T* component_button_edit;
    component_button_T* component_button_delete;
    component_button_T* component_button_new;
} scene_script_selector_T;


scene_script_selector_T* init_scene_script_selector();

void scene_script_selector_tick(scene_T* self);

void scene_script_selector_draw(scene_T* self);

void scene_script_selector_refresh_state(scene_base_T* scene_base);
#endif
