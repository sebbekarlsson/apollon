#ifndef APOLLON_SCENE_ACTOR_EDITOR_H
#define APOLLON_SCENE_ACTOR_EDITOR_H
#include <coelum/dynamic_list.h>
#include <athena/database.h>
#include "scene_base.h"
#include "component_input_field.h"
#include "component_dropdown_list.h"
#include "component_label.h"
#include "component_button.h"


typedef struct SCENE_ACTOR_EDITOR_STRUCT
{
    scene_base_T base;

    // the index of the current actor in database that is being edited.
    char* actor_definition_id;

    /* ==== LEFt ==== */

    /* ==== actor ==== */
    component_label_T* component_label_actor;
    component_dropdown_list_T* component_dropdown_list_actor;

    /* ==== init_script ==== */
    component_label_T* component_label_init_script;
    component_dropdown_list_T* component_dropdown_list_init_script;

    /* ==== tick_script ==== */
    component_label_T* component_label_tick_script;
    component_dropdown_list_T* component_dropdown_list_tick_script;

    /* ==== draw_script ==== */
    component_label_T* component_label_draw_script;
    component_dropdown_list_T* component_dropdown_list_draw_script;
    
    /* ==== new actor component_button ==== */
    component_button_T* component_button_new_actor;


    /* ==== RIGHT ==== */

    /* ==== type_name ==== */
    component_label_T* component_label_type_name;
    component_input_field_T* component_input_field_type_name;

    /* ==== sprite ==== */
    component_label_T* component_label_sprite;
    component_dropdown_list_T* component_dropdown_list_sprite; 

    /* ==== save component_button ==== */
    component_button_T* component_button_save;

    /* ==== delete component_button ==== */
    component_button_T* component_button_delete;
} scene_actor_editor_T;

scene_actor_editor_T* init_scene_actor_editor();

void scene_actor_editor_tick(scene_T* self);

void scene_actor_editor_draw(scene_T* self);

void scene_actor_editor_refresh_state(scene_base_T* scene_base);
#endif
