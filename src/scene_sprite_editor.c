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
        0,
        "grid_canvas"
    );

    // this one is starts as focused
    s_sprite_editor->grid->focused = 1;

    s_sprite_editor->grids = init_dynamic_list(sizeof(struct GRID_STRUCT));

    s_sprite_editor->grid_color_selector = init_grid(
        ((WINDOW_WIDTH / 2) - ((16 * 16) / 2)) + ((16 * 16) + 16),
        ((WINDOW_HEIGHT / 2) - ((16 * 16) / 2)),
        0.0f,
        4,
        8,
        16,
        0,
        0,
        0,
        "grid_color_selector"
    );

    // state representation of first grid in frame list.
    dynamic_list_append(
        s_sprite_editor->grids,
        init_grid(
            (WINDOW_WIDTH / 2) - ((16 * 16) / 2),
            (WINDOW_HEIGHT / 2) - ((16 * 16) / 2),
            0.0f,
            16,
            16,
            16,
            0,
            0,
            0,
            "grid_canvas"
        )
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
        0,
        "grid_color_mixer"
    );

    dynamic_list_append(state->actors, s_sprite_editor->grid);
    dynamic_list_append(state->actors, s_sprite_editor->grid_color_selector);
    dynamic_list_append(state->actors, s_sprite_editor->grid_color_mixer);

    s_sprite_editor->focusables = init_dynamic_list(sizeof(struct GRID_STRUCT));

    dynamic_list_append(s_sprite_editor->focusables, s_sprite_editor->grid);
    dynamic_list_append(s_sprite_editor->focusables, s_sprite_editor->grid_color_selector);
    dynamic_list_append(s_sprite_editor->focusables, s_sprite_editor->grid_color_mixer);

    s_sprite_editor->focus_index = 0;
    s_sprite_editor->grid_index = 0;

    s_sprite_editor->r = 0.0f;
    s_sprite_editor->g = 0.0f;
    s_sprite_editor->b = 0.0f;

    return s_sprite_editor;
}

void scene_sprite_editor_tick(scene_T* self)
{
    scene_sprite_editor_T* s_sprite_editor = (scene_sprite_editor_T*) self;

    if (KEYBOARD_STATE->keys[GLFW_KEY_TAB] && !KEYBOARD_STATE->key_locks[GLFW_KEY_TAB])
    {
        if (s_sprite_editor->focus_index < s_sprite_editor->focusables->size - 1)
        {
            s_sprite_editor->focus_index += 1;
        }
        else
        {
            s_sprite_editor->focus_index = 0;
        }

        KEYBOARD_STATE->key_locks[GLFW_KEY_TAB] = 1;
    }

    for (int i = 0; i < s_sprite_editor->focusables->size; i++)
        ((grid_T*)s_sprite_editor->focusables->items[i])->focused = 0;

    grid_T* grid = (grid_T*) s_sprite_editor->focusables->items[s_sprite_editor->focus_index];
    actor_T* grid_actor = (actor_T*) grid;
    grid->focused = 1;

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

    if (KEYBOARD_STATE->keys[GLFW_KEY_Z] && !KEYBOARD_STATE->key_locks[GLFW_KEY_Z])
    {
        scene_sprite_editor_goto_prev(s_sprite_editor);
        KEYBOARD_STATE->key_locks[GLFW_KEY_Z] = 1;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_X] && !KEYBOARD_STATE->key_locks[GLFW_KEY_X])
    {
        scene_sprite_editor_goto_next(s_sprite_editor);
        KEYBOARD_STATE->key_locks[GLFW_KEY_X] = 1;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_S] && !KEYBOARD_STATE->key_locks[GLFW_KEY_S])
    {
        grid_create_image(s_sprite_editor->grid, "sheet.png");
        KEYBOARD_STATE->key_locks[GLFW_KEY_S] = 1;
    }

    if (strcmp(grid_actor->type_name, "grid_canvas") == 0)
    {
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

            grid->cells[grid->cursor_x][grid->cursor_y]->r = s_sprite_editor->r;
            grid->cells[grid->cursor_x][grid->cursor_y]->g = s_sprite_editor->g;
            grid->cells[grid->cursor_x][grid->cursor_y]->b = s_sprite_editor->b;
        }

        // save current state of the grid into the representation in our state list.
        grid_copy(
            s_sprite_editor->grid,
            s_sprite_editor->grids->items[s_sprite_editor->grid_index]
        );
    }
    else
    if (strcmp(grid_actor->type_name, "grid_color_selector") == 0 || strcmp(grid_actor->type_name, "grid_color_mixer") == 0)
    {
        if (KEYBOARD_STATE->keys[GLFW_KEY_G])
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

            s_sprite_editor->r = grid->cells[grid->cursor_x][grid->cursor_y]->r;
            s_sprite_editor->g = grid->cells[grid->cursor_x][grid->cursor_y]->g;
            s_sprite_editor->b = grid->cells[grid->cursor_x][grid->cursor_y]->b;
        }
    }

    if (strcmp(grid_actor->type_name, "grid_color_mixer") == 0)
    {
        if (KEYBOARD_STATE->keys[GLFW_KEY_SPACE] && ! KEYBOARD_STATE->key_locks[GLFW_KEY_SPACE])
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

            if (
                grid->cells[grid->cursor_x][grid->cursor_y]->r == 255 &&
                grid->cells[grid->cursor_x][grid->cursor_y]->g == 255 &&
                grid->cells[grid->cursor_x][grid->cursor_y]->b == 255
            )
            {
                grid->cells[grid->cursor_x][grid->cursor_y]->r = 0;
                grid->cells[grid->cursor_x][grid->cursor_y]->g = 0;
                grid->cells[grid->cursor_x][grid->cursor_y]->b = 0;
            }

            grid->cells[grid->cursor_x][grid->cursor_y]->r = (s_sprite_editor->r + grid->cells[grid->cursor_x][grid->cursor_y]->r);
            grid->cells[grid->cursor_x][grid->cursor_y]->g = (s_sprite_editor->g + grid->cells[grid->cursor_x][grid->cursor_y]->g);
            grid->cells[grid->cursor_x][grid->cursor_y]->b = (s_sprite_editor->b + grid->cells[grid->cursor_x][grid->cursor_y]->b);

            KEYBOARD_STATE->key_locks[GLFW_KEY_SPACE] = 1;
        }
    }
}

void scene_sprite_editor_draw(scene_T* self)
{
}

void scene_sprite_editor_goto_next(scene_sprite_editor_T* self)
{
    self->grid_index += 1;

    if (self->grids->size-1 < self->grid_index)
    {
        printf("Created another grid state representation!\n");

        dynamic_list_append(
            self->grids,
            init_grid(
                (WINDOW_WIDTH / 2) - ((16 * 16) / 2),
                (WINDOW_HEIGHT / 2) - ((16 * 16) / 2),
                0.0f,
                16,
                16,
                16,
                0,
                0,
                0,
                "grid_canvas"
            )
        );
    }

    scene_sprite_editor_refresh_grid(self);
}

void scene_sprite_editor_goto_prev(scene_sprite_editor_T* self)
{
    if (self->grid_index > 0)
        self->grid_index -= 1;

    scene_sprite_editor_refresh_grid(self);
}

void scene_sprite_editor_refresh_grid(scene_sprite_editor_T* self)
{
    grid_T* current_grid_state = (grid_T*) self->grids->items[self->grid_index];

    if (!current_grid_state)
    {
        printf("Current grid state is broken\n");
        exit(1);
    }

    grid_copy(current_grid_state, self->grid);
}
