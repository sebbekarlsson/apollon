#include "include/scene_actor_editor.h"
#include "include/etc.h"
#include "include/dropdown_list.h"
#include "include/modal_manager.h"
#include "include/main.h"
#include <coelum/input.h>
#include <coelum/current.h>
#include <string.h>


extern keyboard_state_T* KEYBOARD_STATE;
extern database_T* DATABASE;
extern modal_manager_T* MODAL_MANAGER;
extern main_state_T* MAIN_STATE;


static void _free_actor_dropdown_option(void* item)
{
    dropdown_list_option_T* dropdown_list_option = (dropdown_list_option_T*) item;
    
    free(dropdown_list_option->key);
    free((char*)dropdown_list_option->value);
    free(dropdown_list_option);
}

static void scene_actor_editor_clear_input_fields(scene_actor_editor_T* s_actor_editor)
{
    memset(
        s_actor_editor->input_field_type_name->value,
        0,
        sizeof(char) * strlen(s_actor_editor->input_field_type_name->value)
    );
}

void scene_actor_editor_refresh_state(scene_base_T* scene_base)
{
    scene_actor_editor_T* s_actor_editor = (scene_actor_editor_T*) scene_base;

    printf("Refreshing state...\n");

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
        5
    );

    dropdown_list_sync_from_table(
        s_actor_editor->dropdown_list_init_script,
        DATABASE,
        "scripts",
        1,
        -1
    );

    dropdown_list_sync_from_table(
        s_actor_editor->dropdown_list_tick_script,
        DATABASE,
        "scripts",
        1,
        -1
    );

    dropdown_list_sync_from_table(
        s_actor_editor->dropdown_list_draw_script,
        DATABASE,
        "scripts",
        1,
        -1
    );

    dropdown_list_reload_sprites(s_actor_editor->dropdown_list_actor);
    dropdown_list_reload_sprites(s_actor_editor->dropdown_list_sprite);

    if (s_actor_editor->actor_definition_id != (void*) 0)
    {
        dropdown_list_set_selected_option_by_string_value(
            s_actor_editor->dropdown_list_actor,
            (const char*) s_actor_editor->actor_definition_id 
        ); 
    }
    else
    {
        scene_actor_editor_clear_input_fields(s_actor_editor);
    }
}

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

    scene_actor_editor_clear_input_fields(s_actor_editor);
}

void button_save_press()
{
    printf("button_save_press\n");

    scene_T* scene = get_current_scene();
    scene_actor_editor_T* s_actor_editor = (scene_actor_editor_T*) scene;

    if (!strlen(s_actor_editor->input_field_type_name->value))
    {
        modal_manager_show_modal(MODAL_MANAGER, "error", "You need to enter a name.");
        return;
    }

    dropdown_list_option_T* option_selected_sprite = dropdown_list_get_selected_option(
        s_actor_editor->dropdown_list_sprite
    );

    if (option_selected_sprite == (void*) 0)
    {
        printf("No sprite selected!\n");
        return;
    }

    char* option_init_script_value = (void*) 0;
    char* option_tick_script_value = (void*) 0;
    char* option_draw_script_value = (void*) 0;

    dropdown_list_option_T* option_init_script = dropdown_list_get_selected_option(s_actor_editor->dropdown_list_init_script);
    dropdown_list_option_T* option_tick_script = dropdown_list_get_selected_option(s_actor_editor->dropdown_list_tick_script);
    dropdown_list_option_T* option_draw_script = dropdown_list_get_selected_option(s_actor_editor->dropdown_list_draw_script);

    if (option_init_script != (void*) 0)
        if (option_init_script->value != (void*) 0)
            option_init_script_value = (char*) option_init_script->value;

    if (option_tick_script != (void*) 0)
        if (option_tick_script->value != (void*) 0)
            option_tick_script_value = (char*) option_tick_script->value;

    if (option_draw_script != (void*) 0)
        if (option_draw_script->value != (void*) 0)
            option_draw_script_value = (char*) option_draw_script->value;


    if (option_init_script_value == (void*)0)
        printf("init void\n");

    if (option_tick_script_value == (void*)0)
        printf("tick void\n");

    if (option_draw_script_value == (void*)0)
        printf("draw void\n");
    
    if (s_actor_editor->actor_definition_id == (void*)0)
    {
        printf("Insert new actor.\n"); 

        char* actor_definition_id = database_insert_actor_definition(
            DATABASE,
            s_actor_editor->input_field_type_name->value,
            (char*) option_selected_sprite->value,
            option_init_script_value,
            option_tick_script_value,
            option_draw_script_value
        );

        s_actor_editor->actor_definition_id = actor_definition_id;
    }
    else
    {
        printf("Edit existing actor %s.\n", s_actor_editor->actor_definition_id);

        database_update_actor_definition_by_id(
            DATABASE,
            s_actor_editor->actor_definition_id,
            s_actor_editor->input_field_type_name->value,
            (char*) option_selected_sprite->value,
            option_init_script_value,
            option_tick_script_value,
            option_draw_script_value        
        );
    }

    REFRESH_STATE(s_actor_editor);
}

void button_delete_press()
{
    printf("Pressed delete\n");

    scene_T* scene = get_current_scene();
    scene_actor_editor_T* s_actor_editor = (scene_actor_editor_T*) scene;

    database_delete_actor_definition_by_id(DATABASE, s_actor_editor->actor_definition_id);

    for (int i = 0; i < s_actor_editor->dropdown_list_actor->options->size; i++)
    {
        dropdown_list_option_T* option = s_actor_editor->dropdown_list_actor->options->items[i];

        if (option->value == (void*)0)
            continue;
        
        char* db_actor_definition_id = (char*) option->value;

        if (strcmp(db_actor_definition_id, s_actor_editor->actor_definition_id) == 0)
        {
            dynamic_list_remove(
                s_actor_editor->dropdown_list_actor->options,
                option,
                _free_actor_dropdown_option 
            );

            break;
        } 
    }

    scene_actor_editor_reset_actor_definition_id(s_actor_editor);

    REFRESH_STATE(s_actor_editor);
}

void actor_editor_sprite_press(void* dropdown_list, void* option) { /* silence */ }

void actor_editor_actor_press(void* dropdown_list, void* option)
{
    printf("Press actor dropdown\n");
    dropdown_list_option_T* dropdown_list_option = (dropdown_list_option_T*) option;

    scene_T* scene = get_current_scene();
    scene_actor_editor_T* s_actor_editor = (scene_actor_editor_T*) scene;

    char* option_value = (char*) dropdown_list_option->value;
    char* actor_definition_id_new = calloc(strlen(option_value) + 1, sizeof(char));
    strcpy(actor_definition_id_new, option_value);

    s_actor_editor->actor_definition_id = actor_definition_id_new;

    database_actor_definition_T* database_actor_definition = database_get_actor_definition_by_id(
        DATABASE,
        s_actor_editor->actor_definition_id 
    );

    s_actor_editor->input_field_type_name->value = realloc(
        s_actor_editor->input_field_type_name->value,
        (strlen(database_actor_definition->name) + 1) * sizeof(char)
    );
    strcpy(s_actor_editor->input_field_type_name->value, database_actor_definition->name);

    dropdown_list_set_selected_option_by_string_value(
        s_actor_editor->dropdown_list_sprite,
        (const char*) database_actor_definition->sprite_id        
    );

    dropdown_list_set_selected_option_by_string_value(
        s_actor_editor->dropdown_list_init_script,
        (const char*) database_actor_definition->init_script_id
    );

    dropdown_list_set_selected_option_by_string_value(
        s_actor_editor->dropdown_list_init_script,
        (const char*) database_actor_definition->tick_script_id
    );

    dropdown_list_set_selected_option_by_string_value(
        s_actor_editor->dropdown_list_init_script,
        (const char*) database_actor_definition->draw_script_id
    );
}

void scene_actor_editor_load(void* self)
{
    scene_T* scene = (scene_T*) self;
    scene_actor_editor_T* s_actor_editor = (scene_actor_editor_T*) scene;
    REFRESH_STATE(s_actor_editor);
}

scene_actor_editor_T* init_scene_actor_editor()
{
    scene_actor_editor_T* s_actor_editor = calloc(1, sizeof(struct SCENE_ACTOR_EDITOR_STRUCT));
    scene_T* scene = (scene_T*) s_actor_editor;
    state_T* state = (state_T*) scene;
    scene_constructor(scene, scene_actor_editor_tick, scene_actor_editor_draw, 2);
    scene_base_T* scene_base = (scene_base_T*) scene;
    scene_base_constructor(scene_base, scene_actor_editor_refresh_state, "Actor Editor");

    scene->load = scene_actor_editor_load;

    scene->type_name = "actor_editor";

    scene->bg_r = 255;
    scene->bg_g = 255;
    scene->bg_b = 255;

    s_actor_editor->actor_definition_id = (void*) 0;

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
    s_actor_editor->dropdown_list_actor->width = 220;
    ((actor_T*)s_actor_editor->dropdown_list_actor)->z = 1;

    dynamic_list_append(state->actors, s_actor_editor->label_actor);
    scene_base_register_focusable(scene_base, (actor_focusable_T*) s_actor_editor->dropdown_list_actor);
    iy += margin;

    /* ==== init_script ====*/
    s_actor_editor->label_init_script = init_label(ix, iy, 0.0f, "Init Script");
    iy += label_margin;
    s_actor_editor->dropdown_list_init_script = init_dropdown_list(ix, iy, 0.0f, (void*)0);
    s_actor_editor->dropdown_list_init_script->width = s_actor_editor->dropdown_list_actor->width;
    dynamic_list_append(state->actors, s_actor_editor->label_init_script);
    scene_base_register_focusable(scene_base, (actor_focusable_T*) s_actor_editor->dropdown_list_init_script);
    iy += margin;

    /* ==== tick_script ====*/
    s_actor_editor->label_tick_script = init_label(ix, iy, 0.0f, "Tick Script");
    iy += label_margin;
    s_actor_editor->dropdown_list_tick_script = init_dropdown_list(ix, iy, 0.0f, (void*)0);
    s_actor_editor->dropdown_list_tick_script->width = s_actor_editor->dropdown_list_actor->width;
    dynamic_list_append(state->actors, s_actor_editor->label_tick_script);
    scene_base_register_focusable(scene_base, (actor_focusable_T*) s_actor_editor->dropdown_list_tick_script);
    iy += margin;

    /* ==== draw_script ====*/
    s_actor_editor->label_draw_script = init_label(ix, iy, 0.0f, "Draw Script");
    iy += label_margin;
    s_actor_editor->dropdown_list_draw_script = init_dropdown_list(ix, iy, 0.0f, (void*)0);
    s_actor_editor->dropdown_list_draw_script->width = s_actor_editor->dropdown_list_actor->width;
    dynamic_list_append(state->actors, s_actor_editor->label_draw_script);
    scene_base_register_focusable(scene_base, (actor_focusable_T*) s_actor_editor->dropdown_list_draw_script);
    iy += margin;

    s_actor_editor->button_new_actor = init_button(ix, iy, 0.0f, "New Actor", button_new_actor_press);
    scene_base_register_focusable(scene_base, (actor_focusable_T*) s_actor_editor->button_new_actor);


    /* ==== RIGHT ==== */
    
    float jx = 640 / 2;
    float jy = margin;

    /* ==== type_name ====*/
    s_actor_editor->label_type_name = init_label(jx, jy, 0.0f, "Name");
    jy += label_margin;
    s_actor_editor->input_field_type_name = init_input_field(jx, jy, 0.0f);
    dynamic_list_append(state->actors, s_actor_editor->label_type_name);
    scene_base_register_focusable(scene_base, (actor_focusable_T*) s_actor_editor->input_field_type_name);
    jy += margin;

    
    /* ==== sprite ==== */
    s_actor_editor->label_sprite = init_label(jx, jy, 0.0f, "Sprite");
    jy += label_margin;
    s_actor_editor->dropdown_list_sprite = init_dropdown_list(jx, jy, 0.0f, actor_editor_sprite_press);
    s_actor_editor->dropdown_list_sprite->expanded = 0;
    ((actor_T*)s_actor_editor->dropdown_list_sprite)->z = 1;

    dynamic_list_append(state->actors, s_actor_editor->label_sprite);
    scene_base_register_focusable(scene_base, (actor_focusable_T*) s_actor_editor->dropdown_list_sprite);
    jy += margin; 

    /* ==== save button ====*/
    s_actor_editor->button_save = init_button(jx, jy, 0.0f, "Save", button_save_press);
    scene_base_register_focusable(scene_base, (actor_focusable_T*) s_actor_editor->button_save);
    jy += margin; 


    /* ==== delete button ====*/
    s_actor_editor->button_delete = init_button(jx, jy, 0.0f, "Delete", button_delete_press);
    scene_base_register_focusable(scene_base, (actor_focusable_T*) s_actor_editor->button_delete);
    dynamic_list_append(state->actors, s_actor_editor->button_delete);

    state_resort_actors(state);

    return s_actor_editor;
}

void scene_actor_editor_tick(scene_T* self)
{
    go_back_on_escape();

    scene_actor_editor_T* s_actor_editor = (scene_actor_editor_T*) self;

    ((actor_focusable_T*)s_actor_editor->button_delete)->visible = s_actor_editor->actor_definition_id != (void*)0;
    scene_base_tick((scene_base_T*)self);
}

void scene_actor_editor_draw(scene_T* self)
{
    scene_base_draw((scene_base_T*) self);
}
