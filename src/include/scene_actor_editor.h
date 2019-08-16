#ifndef APOLLON_SCENE_ACTOR_EDITOR_H
#define APOLLON_SCENE_ACTOR_EDITOR_H
#include <coelum/dynamic_list.h>
#include <athena/database.h>
#include "scene_base.h"
#include "input_field.h"
#include "dropdown_list.h"
#include "focus_manager.h"
#include "label.h"
#include "button.h"


typedef struct SCENE_ACTOR_EDITOR_STRUCT
{
    scene_base_T base;

    // the index of the current actor in database that is being edited.
    char* actor_definition_id;

    /* ==== LEFt ==== */

    /* ==== actor ==== */
    label_T* label_actor;
    dropdown_list_T* dropdown_list_actor;

    /* ==== init_script ==== */
    label_T* label_init_script;
    dropdown_list_T* dropdown_list_init_script;

    /* ==== tick_script ==== */
    label_T* label_tick_script;
    dropdown_list_T* dropdown_list_tick_script;

    /* ==== draw_script ==== */
    label_T* label_draw_script;
    dropdown_list_T* dropdown_list_draw_script;
    
    /* ==== new actor button ==== */
    button_T* button_new_actor;


    /* ==== RIGHT ==== */

    /* ==== type_name ==== */
    label_T* label_type_name;
    input_field_T* input_field_type_name;

    /* ==== sprite ==== */
    label_T* label_sprite;
    dropdown_list_T* dropdown_list_sprite; 

    /* ==== save button ==== */
    button_T* button_save;

    /* ==== delete button ==== */
    button_T* button_delete;
} scene_actor_editor_T;

scene_actor_editor_T* init_scene_actor_editor();

void scene_actor_editor_tick(scene_T* self);

void scene_actor_editor_draw(scene_T* self);

void scene_actor_editor_refresh_state(scene_base_T* scene_base);
#endif
