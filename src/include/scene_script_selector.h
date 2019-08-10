#ifndef APOLLON_SCENE_SCRIPT_SELECTOR_H
#define APOLLON_SCENE_SCRIPT_SELECTOR_H
#include <coelum/scene.h>
#include "focus_manager.h"
#include "dropdown_list.h"
#include "input_field.h"
#include "button.h"
#include "label.h"


typedef struct SCENE_SCRIPT_SELECTOR_STRUCT
{
    scene_T base;
    focus_manager_T* focus_manager;
    char* script_id;
    database_script_T* current_database_script;

    /** LEFT **/
    label_T* label_script;
    dropdown_list_T* dropdown_list_script;

    /** RIGHT **/
    label_T* label_name;
    input_field_T* input_field_name;
    button_T* button_save;
    button_T* button_edit;
    button_T* button_delete;
    button_T* button_new;
} scene_script_selector_T;


scene_script_selector_T* init_scene_script_selector();

void scene_script_selector_tick(scene_T* self);

void scene_script_selector_draw(scene_T* self);
#endif
