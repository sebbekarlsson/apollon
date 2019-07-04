#include "include/scene_scene_editor.h"
#include "include/etc.h"
#include <coelum/constants.h>
#include <coelum/actor_text.h>
#include <coelum/input.h>
#include <coelum/utils.h>
#include <coelum/draw_utils.h>
#include <string.h>


extern keyboard_state_T* KEYBOARD_STATE;

scene_scene_editor_T* init_scene_scene_editor()
{
    scene_scene_editor_T* s_scene_editor = calloc(1, sizeof(struct SCENE_SCENE_EDITOR_STRUCT));
    scene_T* s = (scene_T*) s_scene_editor;
    state_T* state = (state_T*) s; 

    scene_constructor(s, scene_scene_editor_tick, scene_scene_editor_draw, 2);

    s->type_name = "scene_editor";
    s->bg_r = 255;
    s->bg_g = 255;
    s->bg_b = 255;

    s_scene_editor->grid = init_grid(
        16.0f,
        16.0f,
        0.0f,
        (WINDOW_WIDTH / 16) - 2,
        (WINDOW_HEIGHT / 16) - 2,
        16,
        0,
        0,
        0,
        "grid"
    );

    // this one is starts as focused
    s_scene_editor->grid->focused = 1;
    s_scene_editor->dropdown_list = init_dropdown_list(0.0f, 0.0f, 0.0f);
    dynamic_list_append(s_scene_editor->dropdown_list->options, init_dropdown_list_option("item 0", (void*) 0));
    dynamic_list_append(s_scene_editor->dropdown_list->options, init_dropdown_list_option("item 1", (void*) 0));
    dynamic_list_append(s_scene_editor->dropdown_list->options, init_dropdown_list_option("item 2", (void*) 0));
    
    dynamic_list_append(state->actors, s_scene_editor->grid);
    dynamic_list_append(state->actors, s_scene_editor->dropdown_list);

    return s_scene_editor;
}

void scene_scene_editor_tick(scene_T* self)
{
    go_back_on_escape();

    scene_scene_editor_T* s_scene_editor = (scene_scene_editor_T*) self;
    grid_T* grid = s_scene_editor->grid;

    if (KEYBOARD_STATE->keys[GLFW_KEY_UP] && !KEYBOARD_STATE->key_locks[GLFW_KEY_UP])
    {
        grid->cursor_y -= 1;
        KEYBOARD_STATE->key_locks[GLFW_KEY_UP] = 1;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_DOWN] && !KEYBOARD_STATE->key_locks[GLFW_KEY_DOWN])
    {
        grid->cursor_y += 1;
        KEYBOARD_STATE->key_locks[GLFW_KEY_DOWN] = 1;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_LEFT] && !KEYBOARD_STATE->key_locks[GLFW_KEY_LEFT])
    {
        grid->cursor_x -= 1;
        KEYBOARD_STATE->key_locks[GLFW_KEY_LEFT] = 1;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_RIGHT] && !KEYBOARD_STATE->key_locks[GLFW_KEY_RIGHT])
    {
        grid->cursor_x += 1;
        KEYBOARD_STATE->key_locks[GLFW_KEY_RIGHT] = 1;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_I] && !KEYBOARD_STATE->key_locks[GLFW_KEY_I])
    {
        ((actor_T*)s_scene_editor->dropdown_list)->x = (grid->cursor_x * grid->cell_size) + grid->cell_size * 2;
        ((actor_T*)s_scene_editor->dropdown_list)->y = grid->cursor_y * grid->cell_size + grid->cell_size * 2;

        if (!s_scene_editor->dropdown_list->focused)
            s_scene_editor->dropdown_list->focused = 1;
        else
            s_scene_editor->dropdown_list->focused = 0;

        KEYBOARD_STATE->key_locks[GLFW_KEY_I] = 1;
    }
}

void scene_scene_editor_draw(scene_T* self)
{
}
