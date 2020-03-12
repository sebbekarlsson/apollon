#include "include/main.h"
#include "include/scene_scene_editor.h"
#include "include/etc.h"
#include "include/modal_manager.h"
#include "include/constants.h"
#include <coelum/input.h>
#include <coelum/theatre.h>
#include <coelum/current.h>
#include <string.h>


extern keyboard_state_T* KEYBOARD_STATE;
extern database_T* DATABASE;
extern theatre_T* THEATRE;
extern main_state_T* MAIN_STATE;
extern modal_manager_T* MODAL_MANAGER;
extern const float COLOR_FG[3];


static void _free_scene_component_dropdown_option(void* item)
{
    component_dropdown_list_option_T* component_dropdown_list_option = (component_dropdown_list_option_T*) item;
    
    free(component_dropdown_list_option->key);
    free((char*)component_dropdown_list_option->value);
    free(component_dropdown_list_option);
}

static void scene_scene_editor_clear_component_input_fields(scene_scene_editor_T* s_scene_editor)
{
    memset(
        s_scene_editor->component_input_field_name->value,
        0,
        sizeof(char) * strlen(s_scene_editor->component_input_field_name->value)
    );
}

void scene_scene_editor_unload(void* self)
{
    scene_T* scene = (scene_T*) self;
    scene_scene_editor_T* s_scene_editor = (scene_scene_editor_T*) scene;

    ((scene_base_T*)s_scene_editor)->focus_manager->focus_index = -1;
}

void scene_scene_editor_refresh_state(scene_base_T* scene_base)
{
    scene_scene_editor_T* s_scene_editor = (scene_scene_editor_T*) scene_base;

    component_dropdown_list_sync_from_table(
        s_scene_editor->component_dropdown_list_scene,
        DATABASE,
        "scenes",
        1,
        -1
    );

    if (s_scene_editor->scene_id != (void*) 0)
    {
        s_scene_editor->component_button_design->disabled = 0;
    }
    else
    {
        scene_scene_editor_clear_component_input_fields(s_scene_editor);
        s_scene_editor->component_button_design->disabled = 1;
        s_scene_editor->component_checkbox_main_scene->checked = 0;

        component_dropdown_list_reset(
            s_scene_editor->component_dropdown_list_scene
        );
    }
}

void scene_scene_editor_reset_scene_id(scene_scene_editor_T* s_scene_editor)
{
    if (s_scene_editor->scene_id != (void*) 0)
        free(s_scene_editor->scene_id);

    s_scene_editor->scene_id = (void*) 0;
    MAIN_STATE->scene_id = (void*) 0;
}

void scene_editor_scene_press(void* component_dropdown_list, void* option)
{
    component_dropdown_list_option_T* component_dropdown_list_option = (component_dropdown_list_option_T*) option;

    scene_T* scene = get_current_scene();
    scene_scene_editor_T* s_scene_editor = (scene_scene_editor_T*) scene;

    char* option_value = (char*) component_dropdown_list_option->value;

    char* scene_id_new = calloc(strlen(option_value) + 1, sizeof(char));
    strcpy(scene_id_new, option_value);

    s_scene_editor->scene_id = scene_id_new;
    MAIN_STATE->scene_id = scene_id_new;

    database_scene_T* database_scene = database_get_scene_by_id(DATABASE, s_scene_editor->scene_id);

    s_scene_editor->component_input_field_name->value = realloc(
        s_scene_editor->component_input_field_name->value,
        (strlen(database_scene->name) + 1) * sizeof(char)
    );
    strcpy(s_scene_editor->component_input_field_name->value, database_scene->name);

    s_scene_editor->component_checkbox_main_scene->checked = database_scene->main;

    REFRESH_STATE(s_scene_editor);
}

void component_button_scene_new_press()
{
    scene_T* scene = get_current_scene();
    scene_scene_editor_T* s_scene_editor = (scene_scene_editor_T*) scene;

    scene_scene_editor_reset_scene_id(s_scene_editor);

    scene_scene_editor_clear_component_input_fields(s_scene_editor); 

    s_scene_editor->component_checkbox_main_scene->checked = 0;

    REFRESH_STATE(s_scene_editor);
}

void component_button_scene_design_press()
{
    scene_manager_goto(THEATRE->scene_manager, "scene_designer");
}

void component_button_scene_save_press()
{
    scene_T* scene = get_current_scene();
    scene_scene_editor_T* s_scene_editor = (scene_scene_editor_T*) scene;

    if (!strlen(s_scene_editor->component_input_field_name->value))
    {
        modal_manager_show_modal(MODAL_MANAGER, "error", "You need to enter a name.");
        return;
    }
    
    if (s_scene_editor->component_checkbox_main_scene->checked)
        database_unset_main_flag_on_all_scenes(DATABASE);

    if (s_scene_editor->scene_id == (void*) 0)
    {
        printf("Insert new scene.\n");
        s_scene_editor->scene_id = database_insert_scene(
            DATABASE,
            s_scene_editor->component_input_field_name->value,
            s_scene_editor->component_checkbox_main_scene->checked
         );
        MAIN_STATE->scene_id = s_scene_editor->scene_id;
        REFRESH_STATE(s_scene_editor);
    } 
    else
    {
        printf("Update existing scene.\n");
        database_update_scene_by_id(
            DATABASE,
            s_scene_editor->scene_id,
            s_scene_editor->component_input_field_name->value,
            s_scene_editor->component_checkbox_main_scene->checked
        );
        REFRESH_STATE(s_scene_editor);
    }
}

void component_button_scene_delete_press()
{
    printf("Pressed delete\n");

    scene_T* scene = get_current_scene();
    scene_scene_editor_T* s_scene_editor = (scene_scene_editor_T*) scene;

    if (s_scene_editor->scene_id == (void*) 0)
    {
        printf("Not currently modifying a scene.\n");
        return ;
    }

    database_delete_scene_by_id(DATABASE, s_scene_editor->scene_id);

    for (int i = 0; i < s_scene_editor->component_dropdown_list_scene->options->size; i++)
    {
        component_dropdown_list_option_T* option = s_scene_editor->component_dropdown_list_scene->options->items[i];

        if (option->value == (void*)0)
            continue;
        
        char* db_scene_id = (char*) option->value;

        if (strcmp(db_scene_id, s_scene_editor->scene_id) == 0)
        {
            dynamic_list_remove(
                s_scene_editor->component_dropdown_list_scene->options,
                option,
                _free_scene_component_dropdown_option 
            );

            break;
        } 
    }

    scene_scene_editor_reset_scene_id(s_scene_editor);

    REFRESH_STATE(s_scene_editor);
}

scene_scene_editor_T* init_scene_scene_editor()
{
    scene_scene_editor_T* s_scene_editor = calloc(1, sizeof(struct SCENE_SCENE_EDITOR_STRUCT));
    scene_base_T* scene_base = (scene_base_T*) s_scene_editor;
    scene_T* s = (scene_T*) scene_base;
    state_T* state = (state_T*) s; 

    scene_constructor(s, scene_scene_editor_tick, scene_scene_editor_draw, 2);
    scene_base_constructor(scene_base, scene_scene_editor_refresh_state, "Scene Editor");

    s->unload = scene_scene_editor_unload;

    s->type_name = "scene_editor";
    s->bg_r = 255;
    s->bg_g = 255;
    s->bg_b = 255;
    
    s_scene_editor->scene_id = (void*) 0;
    
    component_pane_T* left = init_component_pane(state, scene_base->focus_manager, 0.0f, 0.0f, 0.0f, 0.0f);
    left->centered = 1;
    left->child_margin_top = COMPONENT_PANE_CHILD_MARGIN_TOP;
    component_pane_T* right = init_component_pane(state, scene_base->focus_manager, 0.0f, 0.0f, 0.0f, 0.0f);
    right->centered = 1;
    right->child_margin_top =COMPONENT_PANE_CHILD_MARGIN_TOP;
    
    dynamic_list_append(scene_base->component_pane->cols, left);
    dynamic_list_append(scene_base->component_pane->cols, right);

    /* ==== LEFT ==== */

    /* ==== actor ==== */
    s_scene_editor->component_label_scene = init_component_label(scene_base->focus_manager, 0.0f, 0.0f, 0.0f, "Scene", COLOR_FG[0],
        COLOR_FG[1],
        COLOR_FG[2]);
    ((actor_component_T*)s_scene_editor->component_label_scene)->margin_y = 4;
    s_scene_editor->component_dropdown_list_scene = init_component_dropdown_list(scene_base->focus_manager, 0.0f, 0.0f, 0.0f, scene_editor_scene_press);
    ((actor_component_T*)s_scene_editor->component_dropdown_list_scene)->margin_y = 4;
    s_scene_editor->component_dropdown_list_scene->expanded = 0;
    ((actor_T*)s_scene_editor->component_dropdown_list_scene)->z = 1;

    component_pane_add_component(
        left,
        (actor_component_T*) s_scene_editor->component_label_scene        
    );
    component_pane_add_component(
        left,
        (actor_component_T*) s_scene_editor->component_dropdown_list_scene 
    );

    const char* _number_of_actors_text = "Actors:";
    char* number_of_actors_text = calloc(strlen(_number_of_actors_text) + 1, sizeof(char));
    strcpy(number_of_actors_text, _number_of_actors_text);

    s_scene_editor->component_button_new = init_component_button(scene_base->focus_manager, 0.0f, 0.0f, 0.0f, "New Scene", component_button_scene_new_press);
    ((actor_component_T*)s_scene_editor->component_button_new)->margin_y = 4;
    component_pane_add_component(
        left,
        (actor_component_T*) s_scene_editor->component_button_new 
    );

    /* ==== RIGHT ==== */
    
    /* ==== type_name ====*/
    s_scene_editor->component_label_name = init_component_label(scene_base->focus_manager, 0.0f, 0.0f, 0.0f, "Name", COLOR_FG[0],
        COLOR_FG[1],
        COLOR_FG[2]);
    ((actor_component_T*)s_scene_editor->component_label_name)->margin_y = 4;
    s_scene_editor->component_input_field_name = init_component_input_field(scene_base->focus_manager, 0.0f, 0.0f, 0.0f);
    ((actor_component_T*)s_scene_editor->component_input_field_name)->margin_y = 4;
    component_pane_add_component(
        right,
        (actor_component_T*) s_scene_editor->component_label_name
    );
    component_pane_add_component(
        right,
        (actor_component_T*) s_scene_editor->component_input_field_name
    );

    /* ==== checkbox_main_scene ====*/
    s_scene_editor->component_label_main_scene = init_component_label(scene_base->focus_manager, 0.0f, 0.0f, 0.0f, "Main", COLOR_FG[0],
        COLOR_FG[1],
        COLOR_FG[2]);
    s_scene_editor->component_checkbox_main_scene = init_component_checkbox(scene_base->focus_manager, 0.0f, 0.0f, 0.0f);
    ((actor_component_T*)s_scene_editor->component_label_main_scene)->margin_y = 4;
    ((actor_component_T*)s_scene_editor->component_checkbox_main_scene)->margin_y = 4;
    component_pane_add_component(
        right,
        (actor_component_T*) s_scene_editor->component_label_main_scene
    );
    component_pane_add_component(
        right,
        (actor_component_T*) s_scene_editor->component_checkbox_main_scene
    );
    //scene_base_register_focusable(scene_base, (actor_focusable_T*) s_scene_editor->component_checkbox_main_scene);

    /* ==== design component_button ====*/
    s_scene_editor->component_button_design = init_component_button(scene_base->focus_manager, 0.0f, 0.0f, 0.0f, "Design", component_button_scene_design_press);
    ((actor_component_T*)s_scene_editor->component_button_design)->margin_y = 4;
    component_pane_add_component(
        right,
        (actor_component_T*) s_scene_editor->component_button_design
    );

    /* ==== save component_button ====*/
    s_scene_editor->component_button_save = init_component_button(scene_base->focus_manager, 0.0f, 0.0f, 0.0f, "Save", component_button_scene_save_press);
    ((actor_component_T*)s_scene_editor->component_button_save)->margin_y = 4;
    component_pane_add_component(
        right,
        (actor_component_T*) s_scene_editor->component_button_save
    );

    /* ==== save component_button ====*/
    s_scene_editor->component_button_delete = init_component_button(scene_base->focus_manager, 0.0f, 0.0f, 0.0f, "Delete", component_button_scene_delete_press);
    ((actor_component_T*)s_scene_editor->component_button_delete)->margin_y = 4;
    component_pane_add_component(
        right,
        (actor_component_T*) s_scene_editor->component_button_delete
    );

    REFRESH_STATE(s_scene_editor);

    state_resort_actors(state);

    return s_scene_editor;
}

void scene_scene_editor_tick(scene_T* self)
{
    go_to_menu_on_escape();

    scene_scene_editor_T* s_scene_editor = (scene_scene_editor_T*) self;

    ((actor_focusable_T*)s_scene_editor->component_button_delete)->visible = s_scene_editor->scene_id != (void*) 0;

    scene_base_tick((scene_base_T*)s_scene_editor);
}

void scene_scene_editor_draw(scene_T* self)
{
    scene_base_draw((scene_base_T*) self);
}
