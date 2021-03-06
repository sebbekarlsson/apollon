#include "include/scene_menu.h"
#include "include/bin_utils.h"
#include "include/component_button.h"
#include <coelum/main.h>
#include <coelum/constants.h>
#include <coelum/actor_text.h>
#include <coelum/theatre.h>
#include <coelum/scene_manager.h>
#include <coelum/draw_utils.h>
#include <coelum/io.h>
#include <hermes/hermes_lexer.h>
#include <hermes/hermes_parser.h>
#include <hermes/hermes_runtime.h>
#include <string.h>


extern float COLOR_BG_DARK[3];
extern float COLOR_BG_DARK_BRIGHT[3];
extern float COLOR_RED[3];

extern theatre_T* THEATRE;
extern volatile unsigned int RUNNING;


void press_scene_editor()
{
    scene_manager_goto(THEATRE->scene_manager, "scene_editor");
}

void press_actor_editor()
{
    scene_manager_goto(THEATRE->scene_manager, "actor_editor");
}

void press_sprite_editor()
{
    scene_manager_goto(THEATRE->scene_manager, "sprite_editor");
}

void press_text_editor()
{
    scene_manager_goto(THEATRE->scene_manager, "script_selector");
}

void press_run()
{
    printf("Run\n");
    char* leto_bin_path = 0;

    // TODO: move config parsing to separate file / function.
    hermes_lexer_T* hermes_lexer = init_hermes_lexer(read_file("config.he"));
    hermes_parser_T* parser = init_hermes_parser(hermes_lexer);
    AST_T* scenes_node = hermes_parser_parse(parser, (void*) 0);
    runtime_T* runtime = init_runtime();
    runtime_visit(runtime, scenes_node);

    hermes_scope_T* runtime_scope = get_scope(runtime, scenes_node);

    for (int i = 0; i < runtime_scope->variable_definitions->size; i++)
    {
        AST_T* ast_vardef = (AST_T*) runtime_scope->variable_definitions->items[i];

        if (strcmp(ast_vardef->variable_name, "config") == 0)
        {
            AST_T* ast_obj = ast_vardef->variable_value;
            
            for (int j = 0; j < ast_obj->object_children->size; j++)
            {
                AST_T* ast_obj_var = (AST_T*) ast_obj->object_children->items[j];
                
                if (strcmp(ast_obj_var->variable_name, "leto_bin") == 0)
                {
                    leto_bin_path = ast_obj_var->variable_value->string_value;

                    break;
                }
            }
        }

        break;
    }

    execute_binary(leto_bin_path, "leto");

    printf("leto_bin_path: %s\n", leto_bin_path);
}

void press_build()
{
    printf("Build\n");
}

void press_quit()
{
    RUNNING = 0;
}

scene_menu_T* init_scene_menu()
{
    scene_menu_T* s_menu = calloc(1, sizeof(struct SCENE_MENU_STRUCT));
    scene_base_T* scene_base = (scene_base_T*) s_menu;
    scene_T* s = (scene_T*) scene_base;

    scene_constructor(s, scene_menu_tick, scene_menu_draw, 2);
    scene_base_constructor(scene_base, (void*)0, "Menu");

    scene_base->component_pane->centered = 1;
    scene_base->component_pane->child_margin_top = 8;

    s->type_name = "menu";
    s->bg_r = 255;
    s->bg_g = 255;
    s->bg_b = 255;

    component_button_T* button_scene_editor = init_component_button(
        scene_base->focus_manager,
        0.0f, 0.0f, 0.0f,
        "Scene Editor",
        press_scene_editor
    );

    component_pane_add_component(
        scene_base->component_pane,
        (actor_component_T*) button_scene_editor        
    );

    component_button_T* button_actor_editor = init_component_button(
        scene_base->focus_manager,
        0.0f, 0.0f, 0.0f,
        "Actor Editor",
        press_actor_editor 
    );

    component_pane_add_component(
        scene_base->component_pane,
        (actor_component_T*) button_actor_editor        
    );

    component_button_T* button_sprite_editor = init_component_button(
        scene_base->focus_manager,
        0.0f, 0.0f, 0.0f,
        "Sprite Editor",
        press_sprite_editor 
    );

    component_pane_add_component(
        scene_base->component_pane,
        (actor_component_T*) button_sprite_editor        
    );

    component_button_T* button_text_editor = init_component_button(
        scene_base->focus_manager,
        0.0f, 0.0f, 0.0f,
        "Text Editor",
        press_text_editor
    );

    component_pane_add_component(
        scene_base->component_pane,
        (actor_component_T*) button_text_editor        
    );

    component_button_T* button_run = init_component_button(
        scene_base->focus_manager,
        0.0f, 0.0f, 0.0f,
        "Run",
        press_run 
    );

    component_pane_add_component(
        scene_base->component_pane,
        (actor_component_T*) button_run        
    );

    component_button_T* button_build = init_component_button(
        scene_base->focus_manager,
        0.0f, 0.0f, 0.0f,
        "Build",
        press_build 
    );

    component_pane_add_component(
        scene_base->component_pane,
        (actor_component_T*) button_build        
    );

    component_button_T* button_quit = init_component_button(
        scene_base->focus_manager,
        0.0f, 0.0f, 0.0f,
        "Quit",
        press_quit 
    );

    component_pane_add_component(
        scene_base->component_pane,
        (actor_component_T*) button_quit  
    );

    return s_menu;
}

void scene_menu_tick(scene_T* self)
{
    scene_base_tick((scene_base_T*) self);
}

void scene_menu_draw(scene_T* self)
{
    scene_base_draw((scene_base_T*) self);
}
