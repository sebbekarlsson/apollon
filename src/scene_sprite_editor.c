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
        (WINDOW_WIDTH / 2) - ((16 * 16) / 2),
        (WINDOW_HEIGHT / 2) - ((16 * 16) / 2),
        0.0f,
        16,
        16,
        16,
        0,
        0,
        0       
    );

    // this one is starts as focused
    s_sprite_editor->grid->focused = 1;

    s_sprite_editor->grid_color_selector = init_grid(
        ((WINDOW_WIDTH / 2) - ((16 * 16) / 2)) + ((16 * 16) + 16),
        ((WINDOW_HEIGHT / 2) - ((16 * 16) / 2)),
        0.0f,
        4,
        8,
        16,
        0,
        0,
        0       
    );


    float r, g, b = 255.0f;

    for (int x = 0; x < s_sprite_editor->grid_color_selector->width; x++)
    {
        for (int y = 0; y < s_sprite_editor->grid_color_selector->height; y++)
        {
            if (x == 0)
            {
                r = 255 - (y * (255 / 8));
                g = 0;
                b = 0;
            }
            else
            if (x == 1)
            {
                r = 0;
                g = 255 - (y * (255 / 8));
                b = 0;
            }
            else
            if (x == 2)
            {
                r = 0;
                g = 0;
                b = 255 - (y * (255 / 8));
            }
            else
            if (x == 3)
            {
                r = 255 - (y * (255 / 8));
                g = 255 - (y * (255 / 8));
                b = 255 - (y * (255 / 8));
            }

            s_sprite_editor->grid_color_selector->cells[x][y]->r = r;
            s_sprite_editor->grid_color_selector->cells[x][y]->g = g;
            s_sprite_editor->grid_color_selector->cells[x][y]->b = b;
        }
    }

    s_sprite_editor->grid_color_mixer = init_grid(
        ((WINDOW_WIDTH / 2) - ((16 * 16) / 2)) + ((16 * 16) + 16),
        ((WINDOW_HEIGHT / 2) - ((16 * 16) / 2)) + ((16 * 8) + 16),
        0.0f,
        8,
        7,
        16,
        0,
        0,
        0       
    );

    dynamic_list_append(state->actors, s_sprite_editor->grid);
    dynamic_list_append(state->actors, s_sprite_editor->grid_color_selector);
    dynamic_list_append(state->actors, s_sprite_editor->grid_color_mixer);

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
           grid->cursor_x > grid->width - 1 ||
           grid->cursor_y < 0 ||
           grid->cursor_y > grid->height - 1
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
