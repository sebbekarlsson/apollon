#include "include/scene_menu.h"
#include "include/bin_utils.h"
#include <coelum/main.h>
#include <coelum/constants.h>
#include <coelum/actor_text.h>
#include <coelum/theatre.h>
#include <coelum/scene_manager.h>
#include <coelum/draw_utils.h>
#include <hermes/hermes_lexer.h>
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
    coelum_terminate();
}

scene_menu_T* init_scene_menu()
{
    scene_menu_T* s_menu = calloc(1, sizeof(struct SCENE_MENU_STRUCT));
    scene_base_T* scene_base = (scene_base_T*) s_menu;
    scene_T* s = (scene_T*) scene_base;
    //state_T* state = (state_T*) s; 

    scene_constructor(s, scene_menu_tick, scene_menu_draw, 2);
    scene_base_constructor(scene_base, scene_menu_refresh_state, "Menu");

    s->type_name = "menu";
    s->bg_r = 255;
    s->bg_g = 255;
    s->bg_b = 255;

    float button_width = 200;
    float ix = (WINDOW_WIDTH / 2) - (button_width / 2);
    float iy = 96.0f;
    float margin = 42;

    s_menu->button_scene_editor = (button_T*) scene_base_register_focusable(scene_base, (actor_focusable_T*) init_button(ix, iy, 0.0f, "Scene Editor", press_scene_editor));
    iy += margin;
    s_menu->button_actor_editor = (button_T*) scene_base_register_focusable(scene_base, (actor_focusable_T*) init_button(ix, iy, 0.0f, "Actor Editor", press_actor_editor));
    iy += margin;
    s_menu->button_sprite_editor = (button_T*) scene_base_register_focusable(scene_base, (actor_focusable_T*) init_button(ix, iy, 0.0f, "Sprite Editor", press_sprite_editor));
    iy += margin;
    s_menu->button_text_editor = (button_T*) scene_base_register_focusable(scene_base, (actor_focusable_T*) init_button(ix, iy, 0.0f, "Text Editor", press_text_editor));
    iy += margin;
    s_menu->button_run = (button_T*) scene_base_register_focusable(scene_base, (actor_focusable_T*) init_button(ix, iy, 0.0f, "Run", press_run));
    iy += margin;
    s_menu->button_build = (button_T*) scene_base_register_focusable(scene_base, (actor_focusable_T*) init_button(ix, iy, 0.0f, "Build", press_build));
    iy += margin;
    s_menu->button_quit = (button_T*) scene_base_register_focusable(scene_base, (actor_focusable_T*) init_button(ix, iy, 0.0f, "Quit", press_quit));

    return s_menu;
}

void scene_menu_tick(scene_T* self)
{
    scene_base_tick((scene_base_T*) self);
    //scene_menu_T* scene_menu = (scene_menu_T*) self;
}

void scene_menu_draw(scene_T* self)
{
    scene_base_draw((scene_base_T*) self);
    //state_T* state = (state_T*) self;
}

void scene_menu_refresh_state(scene_base_T* scene_base)
{
}
