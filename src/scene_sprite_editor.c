#include "include/scene_sprite_editor.h"
#include <coelum/constants.h>
#include <coelum/actor_text.h>
#include <coelum/input.h>
#include <coelum/utils.h>
#include <string.h>


extern keyboard_state_T* KEYBOARD_STATE;

scene_sprite_editor_T* init_scene_sprite_editor()
{
    scene_sprite_editor_T* s_sprite_editor = calloc(1, sizeof(struct SCENE_SPRITE_EDITOR_STRUCT));
    scene_T* s = (scene_T*) s_sprite_editor;
    state_T* state = (state_T*) s; 

    scene_constructor(s, scene_sprite_editor_tick, scene_sprite_editor_draw, 2);

    s->type_name = "sprite_editor";
    s->bg_r = 255;
    s->bg_g = 255;
    s->bg_b = 255;

    s_sprite_editor->grid = init_grid(
        0.0f,
        0.0f,
        0.0f,
        16,
        16,
        16,
        0,
        0,
        0       
    );

    dynamic_list_append(state->actors, s_sprite_editor->grid);

    return s_sprite_editor;
}

void scene_sprite_editor_tick(scene_T* self)
{
    scene_sprite_editor_T* s_sprite_editor = (scene_sprite_editor_T*) self;
    grid_T* grid = s_sprite_editor->grid;

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

    if (KEYBOARD_STATE->keys[GLFW_KEY_SPACE])
    {
        if (
           grid->cursor_x < 0 ||
           grid->cursor_x > grid->width ||
           grid->cursor_y < 0 ||
           grid->cursor_y > grid->height
        )
        {
            return;
        }

        grid->cells[grid->cursor_x][grid->cursor_y]->r = random_int(0, 255);
        grid->cells[grid->cursor_x][grid->cursor_y]->g = random_int(0, 255);
        grid->cells[grid->cursor_x][grid->cursor_y]->b = random_int(0, 255);
    }
}

void scene_sprite_editor_draw(scene_T* self)
{
}
