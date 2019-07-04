#include "include/scene_menu.h"
#include <coelum/constants.h>
#include <coelum/actor_text.h>
#include <coelum/theatre.h>
#include <coelum/scene_manager.h>
#include <string.h>


extern float COLOR_BG_DARK[3];
extern float COLOR_BG_DARK_BRIGHT[3];

extern theatre_T* THEATRE;

void press()
{
    printf("Press\n");
}

void press_scene_editor()
{
    scene_manager_goto(THEATRE->scene_manager, "scene_editor");
}

void press_sprite_editor()
{
    scene_manager_goto(THEATRE->scene_manager, "sprite_editor");
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
    select_list_register_option(s_menu->select_list, "Actor Editor", press);
    select_list_register_option(s_menu->select_list, "Sprite Editor", press_sprite_editor);
    select_list_register_option(s_menu->select_list, "Quit", press);

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
}
