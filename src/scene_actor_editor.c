#include "include/scene_actor_editor.h"
#include "include/etc.h"
#include "include/database.h"
#include "include/dropdown_list.h"
#include <coelum/input.h>
#include <coelum/current.h>
#include <string.h>


extern keyboard_state_T* KEYBOARD_STATE;
extern database_T* DATABASE;


void scene_actor_editor_reset_actor_definition_id(scene_actor_editor_T* s_actor_editor)
{
    if (s_actor_editor->actor_definition_id != (void*) 0)
    {
        free(s_actor_editor->actor_definition_id);
    }

    s_actor_editor->actor_definition_id = (void*) 0;
}

void button_new_actor_press()
{
    printf("button_new_actor_press\n");

    scene_T* scene = get_current_scene();
    scene_actor_editor_T* s_actor_editor = (scene_actor_editor_T*) scene;
    scene_actor_editor_reset_actor_definition_id(s_actor_editor);

    memset(
        s_actor_editor->input_field_type_name->value,
        0,
        sizeof(char) * strlen(s_actor_editor->input_field_type_name->value)
    );

    memset(
        s_actor_editor->input_field_tick_script->value,
        0,
        sizeof(char) * strlen(s_actor_editor->input_field_tick_script->value)
    );

    memset(
        s_actor_editor->input_field_draw_script->value,
        0,
        sizeof(char) * strlen(s_actor_editor->input_field_draw_script->value)
    );
}

void button_save_press()
{
    printf("button_save_press\n");

    scene_T* scene = get_current_scene();
    scene_actor_editor_T* s_actor_editor = (scene_actor_editor_T*) scene;

    dropdown_list_option_T* option_selected_sprite = dropdown_list_get_selected_option(
        s_actor_editor->dropdown_list_sprite
    );

    if (option_selected_sprite == (void*) 0)
    {
        printf("No sprite selected!\n");
        return;
    }
    
    if (s_actor_editor->actor_definition_id == (void*)0)
    {
        printf("Insert new actor.\n");

        database_insert_actor_definition(
            DATABASE,
            s_actor_editor->input_field_type_name->value,
            (char*) option_selected_sprite->value,
            s_actor_editor->input_field_tick_script->value,
            s_actor_editor->input_field_draw_script->value
        );
    }
    else
    {
        printf("Edit existing actor.\n");
    }

    dropdown_list_sync_from_table(
        s_actor_editor->dropdown_list_actor,
        DATABASE,
        "actor_definitions",
        1,
        4
    );
}

void actor_editor_sprite_press(void* dropdown_list, void* option)
{
    //dropdown_list_option_T* dropdown_list_option = (dropdown_list_option_T*) option;

    /*scene_T* scene = get_current_scene();
    scene_actor_editor_T* s_actor_editor = (scene_actor_editor_T*) scene;*/
}

void actor_editor_actor_press(void* dropdown_list, void* option)
{
    printf("Press actor dropdown\n");
    dropdown_list_option_T* dropdown_list_option = (dropdown_list_option_T*) option;

    scene_T* scene = get_current_scene();
    scene_actor_editor_T* s_actor_editor = (scene_actor_editor_T*) scene;

    // s_actor_editor->actor_definition_id = (char*) dropdown_list_option->value;
}

scene_actor_editor_T* init_scene_actor_editor()
{
    scene_actor_editor_T* s_actor_editor = calloc(1, sizeof(struct SCENE_ACTOR_EDITOR_STRUCT));
    scene_T* scene = (scene_T*) s_actor_editor;
    state_T* state = (state_T*) scene;
    scene_constructor(scene, scene_actor_editor_tick, scene_actor_editor_draw, 2);

    scene->type_name = "actor_editor";

    scene->bg_r = 255;
    scene->bg_g = 255;
    scene->bg_b = 255;

    s_actor_editor->actor_definition_id = (void*) 0;

    s_actor_editor->focus_manager = init_focus_manager();
    
    float margin = 64;
    float label_margin = 16; 


    /* ==== LEFT ==== */

    float ix = margin;
    float iy = margin;

    /* ==== actor ==== */
    s_actor_editor->label_actor = init_label(ix, iy, 0.0f, "Actor");
    iy += label_margin;
    s_actor_editor->dropdown_list_actor = init_dropdown_list(ix, iy, 0.0f, actor_editor_actor_press);
    s_actor_editor->dropdown_list_actor->expanded = 0;
    ((actor_T*)s_actor_editor->dropdown_list_actor)->z = 1;

    dynamic_list_append(s_actor_editor->focus_manager->focusables, (actor_focusable_T*) s_actor_editor->dropdown_list_actor);
    dynamic_list_append(state->actors, s_actor_editor->label_actor);
    dynamic_list_append(state->actors, s_actor_editor->dropdown_list_actor);
    iy += margin;

    s_actor_editor->button_new_actor = init_button(ix, iy, 0.0f, "New Actor", button_new_actor_press);
    dynamic_list_append(s_actor_editor->focus_manager->focusables, (actor_focusable_T*) s_actor_editor->button_new_actor);
    dynamic_list_append(state->actors, s_actor_editor->button_new_actor);


    /* ==== RIGHT ==== */
    
    float jx = 640 / 2;
    float jy = margin;

    /* ==== type_name ====*/
    s_actor_editor->label_type_name = init_label(jx, jy, 0.0f, "Name");
    jy += label_margin;
    s_actor_editor->input_field_type_name = init_input_field(jx, jy, 0.0f);
    dynamic_list_append(s_actor_editor->focus_manager->focusables, (actor_focusable_T*) s_actor_editor->input_field_type_name);
    dynamic_list_append(state->actors, s_actor_editor->label_type_name);
    dynamic_list_append(state->actors, s_actor_editor->input_field_type_name);
    jy += margin;

    
    /* ==== sprite ==== */
    s_actor_editor->label_sprite = init_label(jx, jy, 0.0f, "Sprite");
    jy += label_margin;
    s_actor_editor->dropdown_list_sprite = init_dropdown_list(jx, jy, 0.0f, actor_editor_sprite_press);
    s_actor_editor->dropdown_list_sprite->expanded = 0;
    ((actor_T*)s_actor_editor->dropdown_list_sprite)->z = 1;

    dynamic_list_append(s_actor_editor->focus_manager->focusables, (actor_focusable_T*) s_actor_editor->dropdown_list_sprite);
    dynamic_list_append(state->actors, s_actor_editor->label_sprite);
    dynamic_list_append(state->actors, s_actor_editor->dropdown_list_sprite);
    jy += margin;


    /* ==== tick_script ====*/
    s_actor_editor->label_tick_script = init_label(jx, jy, 0.0f, "Tick Script");
    jy += label_margin;
    s_actor_editor->input_field_tick_script = init_input_field(jx, jy, 0.0f);
    dynamic_list_append(s_actor_editor->focus_manager->focusables, (actor_focusable_T*) s_actor_editor->input_field_tick_script);
    dynamic_list_append(state->actors, s_actor_editor->label_tick_script);
    dynamic_list_append(state->actors, s_actor_editor->input_field_tick_script);
    jy += margin;

    /* ==== draw_script ====*/
    s_actor_editor->label_draw_script = init_label(jx, jy, 0.0f, "Draw Script");
    jy += label_margin;
    s_actor_editor->input_field_draw_script = init_input_field(jx, jy, 0.0f);
    dynamic_list_append(s_actor_editor->focus_manager->focusables, (actor_focusable_T*) s_actor_editor->input_field_draw_script);
    dynamic_list_append(state->actors, s_actor_editor->label_draw_script);
    dynamic_list_append(state->actors, s_actor_editor->input_field_draw_script);
    jy += margin;

    /* ==== save button ====*/
    s_actor_editor->button_save = init_button(jx, jy, 0.0f, "Save", button_save_press);
    dynamic_list_append(s_actor_editor->focus_manager->focusables, (actor_focusable_T*) s_actor_editor->button_save);
    dynamic_list_append(state->actors, s_actor_editor->button_save);

    dropdown_list_sync_from_table(
        s_actor_editor->dropdown_list_sprite,
        DATABASE,
        "sprites",
        1,
        0
    );

    dropdown_list_sync_from_table(
        s_actor_editor->dropdown_list_actor,
        DATABASE,
        "actor_definitions",
        1,
        4
    );

    state_resort_actors(state);

    return s_actor_editor;
}

void scene_actor_editor_tick(scene_T* self)
{
    go_back_on_escape();

    scene_actor_editor_T* s_actor_editor = (scene_actor_editor_T*) self;

    focus_manager_tick(s_actor_editor->focus_manager);
}

void scene_actor_editor_draw(scene_T* self)
{
}
