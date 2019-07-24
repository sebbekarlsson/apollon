#include "include/main.h"
#include "include/scene_scene_editor.h"
#include "include/etc.h"
#include <coelum/input.h>
#include <coelum/theatre.h>
#include <coelum/current.h>
#include <string.h>


extern keyboard_state_T* KEYBOARD_STATE;
extern database_T* DATABASE;
extern theatre_T* THEATRE;
extern main_state_T* MAIN_STATE;


void scene_scene_editor_unload(void* self)
{
    scene_T* scene = (scene_T*) self;
    scene_scene_editor_T* s_scene_editor = (scene_scene_editor_T*) scene;

    s_scene_editor->focus_manager->focus_index = -1;
}

void scene_scene_editor_refresh_state(scene_scene_editor_T* s_scene_editor)
{
    dropdown_list_sync_from_table(
        s_scene_editor->dropdown_list_scene,
        DATABASE,
        "scenes",
        1,
        -1
    );
}

void scene_scene_editor_reset_scene_id(scene_scene_editor_T* s_scene_editor)
{
    if (s_scene_editor->scene_id != (void*) 0)
    {
        free(s_scene_editor->scene_id);
    }

    s_scene_editor->scene_id = (void*) 0;
    MAIN_STATE->scene_id = (void*) 0;
}

void scene_editor_scene_press(void* dropdown_list, void* option)
{
    dropdown_list_option_T* dropdown_list_option = (dropdown_list_option_T*) option;

    scene_T* scene = get_current_scene();
    scene_scene_editor_T* s_scene_editor = (scene_scene_editor_T*) scene;

    s_scene_editor->scene_id = (char*) dropdown_list_option->value;
    MAIN_STATE->scene_id = s_scene_editor->scene_id;

    database_scene_T* database_scene = database_get_scene_by_id(DATABASE, s_scene_editor->scene_id);

    s_scene_editor->input_field_name->value = realloc(
        s_scene_editor->input_field_name->value,
        (strlen(database_scene->name) + 1) * sizeof(char)
    );
    strcpy(s_scene_editor->input_field_name->value, database_scene->name);

    s_scene_editor->checkbox_main_scene->checked = database_scene->main;
}

void button_scene_new_press()
{
    scene_T* scene = get_current_scene();
    scene_scene_editor_T* s_scene_editor = (scene_scene_editor_T*) scene;

    scene_scene_editor_reset_scene_id(s_scene_editor);

    memset(
        s_scene_editor->input_field_name->value,
        0,
        sizeof(char) * strlen(s_scene_editor->input_field_name->value)
    );
}

void button_scene_design_press()
{
    scene_manager_goto(THEATRE->scene_manager, "scene_designer");
}

void button_scene_save_press()
{
    scene_T* scene = get_current_scene();
    scene_scene_editor_T* s_scene_editor = (scene_scene_editor_T*) scene;

    if (s_scene_editor->scene_id == (void*) 0)
    {
        printf("Insert new scene.\n");
        s_scene_editor->scene_id = database_insert_scene(
            DATABASE,
            s_scene_editor->input_field_name->value,
            s_scene_editor->checkbox_main_scene->checked
         );
        MAIN_STATE->scene_id = s_scene_editor->scene_id;
        scene_scene_editor_refresh_state(s_scene_editor);
    } 
    else
    {
        printf("Update existing scene.\n");
        database_update_scene_name_by_id(
            DATABASE,
            s_scene_editor->scene_id,
            s_scene_editor->input_field_name->value
        );
        scene_scene_editor_refresh_state(s_scene_editor);
    }
}

scene_scene_editor_T* init_scene_scene_editor()
{
    scene_scene_editor_T* s_scene_editor = calloc(1, sizeof(struct SCENE_SCENE_EDITOR_STRUCT));
    scene_T* s = (scene_T*) s_scene_editor;
    state_T* state = (state_T*) s; 

    scene_constructor(s, scene_scene_editor_tick, scene_scene_editor_draw, 2);

    s->unload = scene_scene_editor_unload;

    s->type_name = "scene_editor";
    s->bg_r = 255;
    s->bg_g = 255;
    s->bg_b = 255;
    
    s_scene_editor->focus_manager = init_focus_manager();
    
    s_scene_editor->scene_id = (void*) 0;

    float margin = 64;
    float label_margin = 16; 


    /* ==== LEFT ==== */

    float ix = margin;
    float iy = margin;

    /* ==== actor ==== */
    s_scene_editor->label_scene = init_label(ix, iy, 0.0f, "Scene");
    iy += label_margin;
    s_scene_editor->dropdown_list_scene = init_dropdown_list(ix, iy, 0.0f, scene_editor_scene_press);
    s_scene_editor->dropdown_list_scene->expanded = 0;
    ((actor_T*)s_scene_editor->dropdown_list_scene)->z = 1;

    dynamic_list_append(s_scene_editor->focus_manager->focusables, (actor_focusable_T*) s_scene_editor->dropdown_list_scene);
    dynamic_list_append(state->actors, s_scene_editor->label_scene);
    dynamic_list_append(state->actors, s_scene_editor->dropdown_list_scene);
    iy += margin;

    s_scene_editor->button_new = init_button(ix, iy, 0.0f, "New Scene", button_scene_new_press);
    dynamic_list_append(s_scene_editor->focus_manager->focusables, (actor_focusable_T*) s_scene_editor->button_new);
    dynamic_list_append(state->actors, s_scene_editor->button_new);


    /* ==== RIGHT ==== */
    
    float jx = 640 / 2;
    float jy = margin;

    /* ==== type_name ====*/
    s_scene_editor->label_name = init_label(jx, jy, 0.0f, "Name");
    jy += label_margin;
    s_scene_editor->input_field_name = init_input_field(jx, jy, 0.0f);
    dynamic_list_append(s_scene_editor->focus_manager->focusables, (actor_focusable_T*) s_scene_editor->input_field_name);
    dynamic_list_append(state->actors, s_scene_editor->label_name);
    dynamic_list_append(state->actors, s_scene_editor->input_field_name);
    jy += margin;

    /* ==== checkbox_main_scene ====*/
    s_scene_editor->label_main_scene = init_label(jx, jy, 0.0f, "Main");
    jy += label_margin;
    s_scene_editor->checkbox_main_scene = init_checkbox(jx, jy, 0.0f);
    dynamic_list_append(s_scene_editor->focus_manager->focusables, (actor_focusable_T*) s_scene_editor->checkbox_main_scene);
    dynamic_list_append(state->actors, s_scene_editor->label_main_scene);
    dynamic_list_append(state->actors, s_scene_editor->checkbox_main_scene);
    jy += margin;

    /* ==== design button ====*/
    s_scene_editor->button_design = init_button(jx, jy, 0.0f, "Design", button_scene_design_press);
    dynamic_list_append(s_scene_editor->focus_manager->focusables, (actor_focusable_T*) s_scene_editor->button_design);
    dynamic_list_append(state->actors, s_scene_editor->button_design);
    jy += margin;

    /* ==== save button ====*/
    s_scene_editor->button_save = init_button(jx, jy, 0.0f, "Save", button_scene_save_press);
    dynamic_list_append(s_scene_editor->focus_manager->focusables, (actor_focusable_T*) s_scene_editor->button_save);
    dynamic_list_append(state->actors, s_scene_editor->button_save);

    scene_scene_editor_refresh_state(s_scene_editor);

    state_resort_actors(state);

    return s_scene_editor;
}

void scene_scene_editor_tick(scene_T* self)
{
    go_back_on_escape();

    scene_scene_editor_T* s_scene_editor = (scene_scene_editor_T*) self;
    
    focus_manager_tick(s_scene_editor->focus_manager);

    //scene_scene_editor_T* s_scene_editor = (scene_scene_editor_T*) self;
}

void scene_scene_editor_draw(scene_T* self)
{
    /*state_T* state = (state_T*) self;
    scene_scene_editor_T* s_scene_editor = (scene_scene_editor_T*) self;*/
}
