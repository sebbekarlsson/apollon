#include "include/scene_actor_editor.h"
#include "include/etc.h"
#include "include/database.h"
#include "include/dropdown_list.h"
#include <coelum/input.h>
#include <coelum/current.h>
#include <string.h>


extern keyboard_state_T* KEYBOARD_STATE;
extern database_T* DATABASE;

void button_new_actor_press()
{
    printf("button_new_actor_press\n");

    scene_T* scene = get_current_scene();
    scene_actor_editor_T* s_actor_editor = (scene_actor_editor_T*) scene;
    s_actor_editor->actor_index = -1;

    memset(
        s_actor_editor->input_field_type_name->value,
        0,
        sizeof(char) * strlen(s_actor_editor->input_field_type_name->value)
    );

    s_actor_editor->selected_database_sprite = (void*) 0;

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

    if (s_actor_editor->actor_index == -1)
    {
        printf("Save new actor\n");

        char* name = calloc(strlen(s_actor_editor->input_field_type_name->value) + 1, sizeof(char));
        strcpy(name, s_actor_editor->input_field_type_name->value);

        char* tick_script = calloc(strlen(s_actor_editor->input_field_tick_script->value) + 1, sizeof(char));
        strcpy(tick_script, s_actor_editor->input_field_tick_script->value);

        char* draw_script = calloc(strlen(s_actor_editor->input_field_draw_script->value) + 1, sizeof(char));
        strcpy(draw_script, s_actor_editor->input_field_draw_script->value);

        database_actor_definition_T* database_actor_definition = init_database_actor_definition(
            s_actor_editor->selected_database_sprite,
            name,
            tick_script,
            draw_script
        );

        dynamic_list_append(DATABASE->actor_definitions, database_actor_definition);
        s_actor_editor->actor_index = DATABASE->actor_definitions->size - 1;
    }
    else
    {
        printf("Modify existing actor\n");
        database_actor_definition_T* database_actor_definition = (database_actor_definition_T*) DATABASE->actor_definitions->items[s_actor_editor->actor_index];

        database_actor_definition->name = realloc(database_actor_definition->name, (strlen(s_actor_editor->input_field_type_name->value) + 1) * sizeof(char));
        strcpy(database_actor_definition->name, s_actor_editor->input_field_type_name->value);

        database_actor_definition->database_sprite = s_actor_editor->selected_database_sprite;

        database_actor_definition->tick_script = realloc(database_actor_definition->tick_script, (strlen(s_actor_editor->input_field_tick_script->value) + 1) * sizeof(char));
        strcpy(database_actor_definition->tick_script, s_actor_editor->input_field_tick_script->value);

        database_actor_definition->draw_script = realloc(database_actor_definition->draw_script, (strlen(s_actor_editor->input_field_draw_script->value) + 1) * sizeof(char));
        strcpy(database_actor_definition->draw_script, s_actor_editor->input_field_draw_script->value);

        printf("Should change sprite to %s\n", s_actor_editor->selected_database_sprite->name);
    }
}

void actor_editor_sprite_press(void* dropdown_list, void* option)
{
    dropdown_list_option_T* dropdown_list_option = (dropdown_list_option_T*) option;

    scene_T* scene = get_current_scene();
    scene_actor_editor_T* s_actor_editor = (scene_actor_editor_T*) scene;

    s_actor_editor->selected_database_sprite = (database_sprite_T*) dropdown_list_option->value;
}

void actor_editor_actor_press(void* dropdown_list, void* option)
{
    printf("Press actor dropdown\n");
    dropdown_list_option_T* dropdown_list_option = (dropdown_list_option_T*) option;
    database_actor_definition_T* database_actor_definition = (database_actor_definition_T*) dropdown_list_option->value;

    scene_T* scene = get_current_scene();
    scene_actor_editor_T* s_actor_editor = (scene_actor_editor_T*) scene;

    for (int i = 0; i < DATABASE->actor_definitions->size; i++)
    {
        database_actor_definition_T* actor_def = (database_actor_definition_T*) DATABASE->actor_definitions->items[i];

        if (database_actor_definition == actor_def)
        {
            s_actor_editor->actor_index = i;

            s_actor_editor->input_field_type_name->value = realloc(s_actor_editor->input_field_type_name->value, (strlen(actor_def->name) + 1) * sizeof(char));
            strcpy(s_actor_editor->input_field_type_name->value, actor_def->name);

            s_actor_editor->input_field_tick_script->value = realloc(s_actor_editor->input_field_tick_script->value, (strlen(actor_def->tick_script) + 1) * sizeof(char));
            strcpy(s_actor_editor->input_field_tick_script->value, actor_def->tick_script);

            s_actor_editor->input_field_draw_script->value = realloc(s_actor_editor->input_field_draw_script->value, (strlen(actor_def->draw_script) + 1) * sizeof(char));
            strcpy(s_actor_editor->input_field_draw_script->value, actor_def->draw_script);

            for (int j = 0; j < s_actor_editor->dropdown_list_sprite->options->size; j++)
            {
                dropdown_list_option_T* dropdown_list_option = (dropdown_list_option_T*) s_actor_editor->dropdown_list_sprite->options->items[j];

                database_sprite_T* database_sprite = (database_sprite_T*) dropdown_list_option->value;

                if (database_sprite == actor_def->database_sprite)
                {
                    printf("selected index is now %d\n", j);
                    s_actor_editor->dropdown_list_sprite->selected_index = (unsigned int) j;
                    break;
                }
            }

            break;
        }
    }
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

    s_actor_editor->actor_index = -1;
    s_actor_editor->selected_database_sprite = (void*) 0;

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

    state_resort_actors(state);

    return s_actor_editor;
}

void scene_actor_editor_tick(scene_T* self)
{
    go_back_on_escape();

    scene_actor_editor_T* s_actor_editor = (scene_actor_editor_T*) self;

    focus_manager_tick(s_actor_editor->focus_manager);


    if (s_actor_editor->dropdown_list_sprite->options->size < DATABASE->sprites->size)
    {
        for (int i = s_actor_editor->dropdown_list_sprite->options->size; i < DATABASE->sprites->size; i++)
        {
            database_sprite_T* database_sprite = DATABASE->sprites->items[i];

            // 20 = (sprite_width(16) + margin(4))
            // 12 = (font_size + font_spacing)
            unsigned int text_limit = ((s_actor_editor->dropdown_list_sprite->width - 20) / (12)) - 1;

            dynamic_list_append(
                s_actor_editor->dropdown_list_sprite->options,
                init_dropdown_list_option(
                    database_sprite->sprite,
                    database_sprite->name,
                    database_sprite,
                    text_limit
                )
            );
        }
    }

    if (s_actor_editor->dropdown_list_actor->options->size < DATABASE->actor_definitions->size)
    {
        for (int i = s_actor_editor->dropdown_list_actor->options->size; i < DATABASE->actor_definitions->size; i++)
        {
            database_actor_definition_T* database_actor_definition = (database_actor_definition_T*) DATABASE->actor_definitions->items[i];

            // 20 = (sprite_width(16) + margin(4))
            // 12 = (font_size + font_spacing)
            unsigned int text_limit = ((s_actor_editor->dropdown_list_actor->width - 20) / (12)) - 1;

            dynamic_list_append(
                s_actor_editor->dropdown_list_actor->options,
                init_dropdown_list_option(
                    database_actor_definition->database_sprite->sprite,
                    database_actor_definition->name,
                    database_actor_definition,
                    text_limit
                )
            );
        }
    }
}

void scene_actor_editor_draw(scene_T* self)
{
}
