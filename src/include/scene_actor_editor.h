#ifndef APOLLON_SCENE_ACTOR_EDITOR_H
#define APOLLON_SCENE_ACTOR_EDITOR_H
#include <coelum/scene.h>
#include <coelum/dynamic_list.h>
#include "input_field.h"
#include "dropdown_list.h"
#include "focus_manager.h"
#include "label.h"
#include "button.h"
#include "database.h"


typedef struct SCENE_ACTOR_EDITOR_STRUCT
{
    scene_T base;

    // the index of the current actor in database that is being edited.
    int actor_index;

    database_sprite_T* selected_database_sprite;

    /* ==== LEFt ==== */

    /* ==== actor ==== */
    label_T* label_actor;
    dropdown_list_T* dropdown_list_actor;
    
    /* ==== new actor button ==== */
    button_T* button_new_actor;


    /* ==== RIGHT ==== */

    /* ==== type_name ==== */
    label_T* label_type_name;
    input_field_T* input_field_type_name;

    /* ==== sprite ==== */
    label_T* label_sprite;
    dropdown_list_T* dropdown_list_sprite;

    /* ==== tick_script ==== */
    label_T* label_tick_script;
    input_field_T* input_field_tick_script;

    /* ==== draw_script ==== */
    label_T* label_draw_script;
    input_field_T* input_field_draw_script;

    /* ==== save button ==== */
    button_T* button_save;

    focus_manager_T* focus_manager;
} scene_actor_editor_T;

scene_actor_editor_T* init_scene_actor_editor();

void scene_actor_editor_tick(scene_T* self);

void scene_actor_editor_draw(scene_T* self);
#endif
