#include "include/scene_menu.h"
#include "include/bin_utils.h"
#include <coelum/main.h>
#include <coelum/constants.h>
#include <coelum/actor_text.h>
#include <coelum/theatre.h>
#include <coelum/scene_manager.h>
#include <coelum/draw_utils.h>
#include <hermes/lexer.h>
#include <hermes/hermes_parser.h>
#include <hermes/hermes_runtime.h>
#include <hermes/io.h>
#include <string.h>


extern float COLOR_BG_DARK[3];
extern float COLOR_BG_DARK_BRIGHT[3];
extern float COLOR_RED[3];

extern theatre_T* THEATRE;


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
    lexer_T* lexer = init_lexer(read_file("config.he"));
    hermes_parser_T* parser = init_hermes_parser(lexer);
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
    coelum_terminate();
}

scene_menu_T* init_scene_menu()
{
    scene_menu_T* s_menu = calloc(1, sizeof(struct SCENE_MENU_STRUCT));
    scene_T* s = (scene_T*) s_menu;
    state_T* state = (state_T*) s; 

    scene_constructor(s, scene_menu_tick, scene_menu_draw, 2);

    s->type_name = "menu";
    s->bg_r = COLOR_BG_DARK[0];
    s->bg_g = COLOR_BG_DARK[1];
    s->bg_b = COLOR_BG_DARK[2]; 

    s_menu->select_list = init_select_list(state);

    select_list_register_option(s_menu->select_list, "Scene Editor", press_scene_editor);
    select_list_register_option(s_menu->select_list, "Actor Editor", press_actor_editor);
    select_list_register_option(s_menu->select_list, "Sprite Editor", press_sprite_editor);
    select_list_register_option(s_menu->select_list, "Text Editor", press_text_editor);
    select_list_register_option(s_menu->select_list, "Run", press_run);
    select_list_register_option(s_menu->select_list, "Build", press_build);
    select_list_register_option(s_menu->select_list, "Quit", press_quit);

    return s_menu;
}

void scene_menu_tick(scene_T* self)
{
    scene_menu_T* scene_menu = (scene_menu_T*) self;
    select_list_T* select_list = scene_menu->select_list;
    select_list_tick(select_list);
}

void scene_menu_draw(scene_T* self)
{
    state_T* state = (state_T*) self;

    char* text = "Coelum";

    float text_size = 32;
    float text_spacing = 24;

    float banner_height = 148;

    draw_positioned_2D_mesh(
        0,
        0,
        0,
        WINDOW_WIDTH,
        banner_height,
        COLOR_BG_DARK_BRIGHT[0],
        COLOR_BG_DARK_BRIGHT[1],
        COLOR_BG_DARK_BRIGHT[2],
        1.0f,
        state
    );

    draw_text(
        text,
        ((WINDOW_WIDTH / 2) - ((strlen(text) * (text_size + text_spacing)) / 2)) + (text_size / 2),
        78,
        0,
        COLOR_RED[0],
        COLOR_RED[1],
        COLOR_RED[2],
        1.0f,
        text_size,
        text_spacing,
        0,
        state
    );
}
