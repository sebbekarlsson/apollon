#include "include/main.h"
#include "include/scene_scene_designer.h"
#include "include/etc.h"
#include "include/database.h"
#include <coelum/constants.h>
#include <coelum/actor_text.h>
#include <coelum/input.h>
#include <coelum/utils.h>
#include <coelum/draw_utils.h>
#include <coelum/current.h>
#include <string.h>


extern keyboard_state_T* KEYBOARD_STATE;
extern database_T* DATABASE;
extern main_state_T* MAIN_STATE;


void scene_scene_designer_refresh_state(scene_scene_designer_T* s_scene_designer)
{
    printf("Refreshing state...\n");

    s_scene_designer->scene_count = database_count_scenes(DATABASE);
    printf("Number of scenes: %d\n", s_scene_designer->scene_count);

    dropdown_list_sync_from_table(
        s_scene_designer->dropdown_list,
        DATABASE,
        "actor_definitions",
        1,
        4
    );
    dropdown_list_reload_sprites(s_scene_designer->dropdown_list);
}

void scene_designer_dropdown_press(void* dropdown_list, void* option)
{
    //scene_T* scene = get_current_scene();
    //scene_scene_designer_T* s_scene_designer = (scene_scene_designer_T*) scene;
}

scene_scene_designer_T* init_scene_scene_designer()
{
    scene_scene_designer_T* s_scene_designer = calloc(1, sizeof(struct SCENE_SCENE_DESIGNER_STRUCT));
    scene_T* s = (scene_T*) s_scene_designer;
    state_T* state = (state_T*) s; 

    scene_constructor(s, scene_scene_designer_tick, scene_scene_designer_draw, 2);

    s->type_name = "scene_designer";
    s->bg_r = 255;
    s->bg_g = 255;
    s->bg_b = 255;

    s_scene_designer->scene_index = 0;
    s_scene_designer->scene_count = 0;

    // this one is starts as focused
    s_scene_designer->dropdown_list = init_dropdown_list(0.0f, 0.0f, 0.0f, scene_designer_dropdown_press);
    ((actor_focusable_T*)s_scene_designer->dropdown_list)->visible = 0;
    ((actor_T*)s_scene_designer->dropdown_list)->z = 1;

    dynamic_list_append(state->actors, s_scene_designer->dropdown_list);

    scene_scene_designer_refresh_state(s_scene_designer);

    return s_scene_designer;
}

void scene_scene_designer_tick(scene_T* self)
{
    go_back_on_escape();

    scene_scene_designer_T* s_scene_designer = (scene_scene_designer_T*) self;

    if (KEYBOARD_STATE->keys[GLFW_KEY_UP] && !KEYBOARD_STATE->key_locks[GLFW_KEY_UP])
    {
        KEYBOARD_STATE->key_locks[GLFW_KEY_UP] = 1;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_DOWN] && !KEYBOARD_STATE->key_locks[GLFW_KEY_DOWN])
    {
        KEYBOARD_STATE->key_locks[GLFW_KEY_DOWN] = 1;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_LEFT] && !KEYBOARD_STATE->key_locks[GLFW_KEY_LEFT])
    {
        KEYBOARD_STATE->key_locks[GLFW_KEY_LEFT] = 1;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_RIGHT] && !KEYBOARD_STATE->key_locks[GLFW_KEY_RIGHT])
    {
        KEYBOARD_STATE->key_locks[GLFW_KEY_RIGHT] = 1;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_I] && !KEYBOARD_STATE->key_locks[GLFW_KEY_I])
    {
        ((actor_T*)s_scene_designer->dropdown_list)->x = WINDOW_WIDTH / 2;
        ((actor_T*)s_scene_designer->dropdown_list)->y = WINDOW_HEIGHT / 2;
        actor_focusable_T* dropdown_list_focusable = (actor_focusable_T*) s_scene_designer->dropdown_list;

        if (!dropdown_list_focusable->focused)
        {
            dropdown_list_focusable->focused = 1;
            ((actor_focusable_T*)s_scene_designer->dropdown_list)->visible = 1;
            printf("Please show dropdown\n");
        }
        else
        {
            dropdown_list_focusable->focused = 0;
            ((actor_focusable_T*)s_scene_designer->dropdown_list)->visible = 0;
            printf("Please dont show dropdown\n");
        }

        KEYBOARD_STATE->key_locks[GLFW_KEY_I] = 1;
    }
}

void scene_scene_designer_draw(scene_T* self)
{
    state_T* state = (state_T*) self;
    //scene_scene_designer_T* s_scene_designer = (scene_scene_designer_T*) self;

    if (MAIN_STATE->scene_id != (void*) 0)
    {
        draw_text(
            MAIN_STATE->scene_id,
            16 + 6,
            16 - 6,
            0,
            0, // r
            0, // g
            0, // b
            6,
            6,
            0,
            state
        );
    }
}
