#include "include/scene_script_selector.h"
#include "include/etc.h"
#include "include/main.h"
#include <coelum/current.h>
#include <coelum/theatre.h>
#include <athena/database.h>
#include <string.h>


extern database_T* DATABASE;
extern main_state_T* MAIN_STATE;
extern theatre_T* THEATRE;


static void scene_script_selector_reset_script_id(scene_script_selector_T* s_script_selector)
{
    if (s_script_selector->current_database_script != (void*) 0)
    {
        database_script_free(s_script_selector->current_database_script);
        s_script_selector->current_database_script = (void*) 0;
    }

    if (s_script_selector->script_id != (void*)0)
    {
        free(s_script_selector->script_id);
        s_script_selector->script_id = (void*)0;
    }
}

static void scene_script_selector_reset_state(scene_script_selector_T* s_script_selector)
{
    scene_script_selector_reset_script_id(s_script_selector);

    memset(
        s_script_selector->input_field_name->value,
        0,
        sizeof(char) * strlen(s_script_selector->input_field_name->value)
    );
    
    REFRESH_STATE(s_script_selector);
}

static void scene_script_selector_set_current_script(
    scene_script_selector_T* s_script_selector,
    const char* script_id
)
{
    scene_script_selector_reset_script_id(s_script_selector);

    char* new_id = calloc(strlen(script_id) + 1, sizeof(char));
    strcpy(new_id, script_id);

    s_script_selector->script_id = new_id;

    s_script_selector->current_database_script = database_get_script_by_id(
        DATABASE,
        new_id
    );

    MAIN_STATE->text_editor_value = s_script_selector->current_database_script->contents;

    REFRESH_STATE(s_script_selector);
}

static void dropdown_list_script_press(void* dropdown_list, void* option)
{
    scene_T* scene = get_current_scene();
    scene_script_selector_T* s_script_selector = (scene_script_selector_T*) scene;

    dropdown_list_option_T* dropdown_list_option = (dropdown_list_option_T*) option;

    scene_script_selector_set_current_script(s_script_selector, (char*) dropdown_list_option->value);
}

static void button_save_press()
{
    scene_T* scene = get_current_scene();
    scene_script_selector_T* s_script_selector = (scene_script_selector_T*) scene;

    // contents
    char* text_editor_value = MAIN_STATE->text_editor_value;

    // name
    char* name = s_script_selector->input_field_name->value;

    // filepath
    const char* filepath_template = "scripts/%s.he";
    char* filepath = calloc(strlen(filepath_template) + strlen(name) + 1, sizeof(char));
    sprintf(filepath, filepath_template, name);

    if (s_script_selector->script_id == (void*)0)
    {
        printf("Create new script\n"); 

        char* id = database_insert_script(DATABASE, name, filepath);
        char* new_id = calloc(strlen(id) + 1, sizeof(char));
        strcpy(new_id, id);

        printf("A new script was created with the id: %s\n", new_id);

        s_script_selector->script_id = new_id;

        if (text_editor_value != (void*)0)
        {
            FILE * fp;
            fp = fopen(filepath, "w");
            fprintf(fp, text_editor_value);
            fclose (fp);
        }

        REFRESH_STATE(s_script_selector);
    }
    else
    {
        printf("Modifying existing script\n");

        if (text_editor_value != (void*)0)
        {
            FILE * fp;
            fp = fopen(filepath, "w");
            fprintf(fp, text_editor_value);
            fclose (fp);
        }
    }
}

static void button_edit_press()
{
    scene_manager_goto(THEATRE->scene_manager, "text_editor");
}

static void button_delete_press()
{
}

static void button_new_press()
{
    scene_T* scene = get_current_scene();
    scene_script_selector_T* s_script_selector = (scene_script_selector_T*) scene;
    scene_script_selector_reset_state(s_script_selector);
}

void scene_script_selector_load(void* s)
{
    printf("loaded\n");
    scene_T* scene = (scene_T*) s;
    scene_script_selector_T* s_script_selector = (scene_script_selector_T*) scene;

    REFRESH_STATE(s_script_selector);
}

scene_script_selector_T* init_scene_script_selector()
{
    scene_script_selector_T* s_script_selector = calloc(1, sizeof(struct SCENE_SCRIPT_SELECTOR_STRUCT));
    scene_base_T* scene_base = (scene_base_T*) s_script_selector;
    scene_T* s = (scene_T*) scene_base;
    state_T* state = (state_T*) s; 

    scene_constructor(s, scene_script_selector_tick, scene_script_selector_draw, 2);
    scene_base_constructor(scene_base, scene_script_selector_refresh_state, "Script Selector");

    s->load = scene_script_selector_load;

    s->type_name = "script_selector";
    s->bg_r = 255;
    s->bg_g = 255;
    s->bg_b = 255; 

    s_script_selector->script_id = (void*) 0;

    float margin = 64;
    float label_margin = 16; 

    /* ==== LEFT ==== */

    float ix = margin;
    float iy = margin;

    s_script_selector->label_script = init_label(
        ix,
        iy,
        0.0f,
        "Script"
    );
    dynamic_list_append(state->actors, s_script_selector->label_script);

    iy += label_margin;

    s_script_selector->dropdown_list_script = init_dropdown_list(
        ix,
        iy,
        0.0f,
        dropdown_list_script_press
    );
    scene_base_register_focusable(scene_base, (actor_focusable_T*) s_script_selector->dropdown_list_script);

    /* ==== END OF LEFT ==== */

    /* ==== RIGHT ==== */

    float jx = 640 / 2;
    float jy = margin;

    s_script_selector->label_name = init_label(
        jx,
        jy,
        0.0f,
        "Name"
    );
    dynamic_list_append(state->actors, s_script_selector->label_name);

    jy += label_margin;

    s_script_selector->input_field_name = init_input_field(
        jx,
        jy,
        0.0f        
    );
    scene_base_register_focusable(scene_base, (actor_focusable_T*) s_script_selector->input_field_name);

    jy += margin;

    s_script_selector->button_save = init_button(
        jx,
        jy,
        0.0f,
        "Save",
        button_save_press        
    );
    scene_base_register_focusable(scene_base, (actor_focusable_T*) s_script_selector->button_save);

    jy += margin;

    s_script_selector->button_edit = init_button(
        jx,
        jy,
        0.0f,
        "Edit",
        button_edit_press        
    );
    scene_base_register_focusable(scene_base, (actor_focusable_T*) s_script_selector->button_edit);

    jy += margin;

    s_script_selector->button_delete = init_button(
        jx,
        jy,
        0.0f,
        "Delete",
        button_delete_press        
    );
    scene_base_register_focusable(scene_base, (actor_focusable_T*) s_script_selector->button_delete);

    jy += margin;

    s_script_selector->button_new = init_button(
        jx,
        jy,
        0.0f,
        "New",
        button_new_press        
    );
    scene_base_register_focusable(scene_base, (actor_focusable_T*) s_script_selector->button_new);

    return s_script_selector;
}

void scene_script_selector_tick(scene_T* self)
{
    go_to_menu_on_escape();
    scene_base_tick((scene_base_T*) self);
}

void scene_script_selector_draw(scene_T* self)
{
    scene_base_draw((scene_base_T*) self);
}

void scene_script_selector_refresh_state(scene_base_T* scene_base)
{
    scene_script_selector_T* s_script_selector = (scene_script_selector_T*) scene_base;
    
    if (s_script_selector->script_id == (void*)0)
    {
        s_script_selector->button_edit->disabled = 1;
        MAIN_STATE->script_id = (void*)0;
    }
    else
    {
        MAIN_STATE->script_id = s_script_selector->script_id;
        s_script_selector->button_edit->disabled = 0;
    }
    
    if (s_script_selector->current_database_script != (void*) 0)
    {
        database_script_T* database_script = s_script_selector->current_database_script;

        s_script_selector->input_field_name->value = realloc(
            s_script_selector->input_field_name->value,
            (strlen(database_script->name) + 1) * sizeof(char)
        );
        strcpy(s_script_selector->input_field_name->value, database_script->name);
    }

    dropdown_list_sync_from_table(
        s_script_selector->dropdown_list_script,
        DATABASE,
        "scripts",
        1,
        -1
    );
}
