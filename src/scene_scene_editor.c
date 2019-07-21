#include "include/scene_scene_editor.h"
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


void scene_scene_editor_refresh_state(scene_scene_editor_T* s_scene_editor)
{
    printf("Refreshing state...\n");

    dropdown_list_sync_from_table(
        s_scene_editor->dropdown_list,
        DATABASE,
        "actor_definitions",
        1,
        4
    );
    dropdown_list_reload_sprites(s_scene_editor->dropdown_list);
}

void scene_editor_dropdown_press(void* dropdown_list, void* option)
{
    //scene_T* scene = get_current_scene();
    //scene_scene_editor_T* s_scene_editor = (scene_scene_editor_T*) scene;
}

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

    s_scene_editor->scene_index = 0;

    // this one is starts as focused
    ((actor_focusable_T*)s_scene_editor->grid)->focused = 1;
    s_scene_editor->dropdown_list = init_dropdown_list(0.0f, 0.0f, 0.0f, scene_editor_dropdown_press);
    ((actor_focusable_T*)s_scene_editor->dropdown_list)->visible = 0;
    ((actor_T*)s_scene_editor->dropdown_list)->z = 1;

    dynamic_list_append(state->actors, s_scene_editor->grid);
    dynamic_list_append(state->actors, s_scene_editor->dropdown_list);

    scene_scene_editor_refresh_state(s_scene_editor);

    //insert_new_scene("main");

    return s_scene_editor;
}

void scene_scene_editor_tick(scene_T* self)
{
    go_back_on_escape();

    scene_scene_editor_T* s_scene_editor = (scene_scene_editor_T*) self;
    grid_T* grid = s_scene_editor->grid;

    if (KEYBOARD_STATE->keys[GLFW_KEY_Z] && !KEYBOARD_STATE->key_locks[GLFW_KEY_Z])
    {
        scene_scene_editor_goto_prev(s_scene_editor);
        KEYBOARD_STATE->key_locks[GLFW_KEY_Z] = 1;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_X] && !KEYBOARD_STATE->key_locks[GLFW_KEY_X])
    {
        scene_scene_editor_goto_next(s_scene_editor);
        KEYBOARD_STATE->key_locks[GLFW_KEY_X] = 1;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_C] && !KEYBOARD_STATE->key_locks[GLFW_KEY_C])
    {
        scene_scene_editor_delete_current_scene(s_scene_editor);
        KEYBOARD_STATE->key_locks[GLFW_KEY_C] = 1;
    }

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
        actor_focusable_T* dropdown_list_focusable = (actor_focusable_T*) s_scene_editor->dropdown_list;

        if (!dropdown_list_focusable->focused)
        {
            dropdown_list_focusable->focused = 1;
            ((actor_focusable_T*)s_scene_editor->dropdown_list)->visible = 1;
            printf("Please show dropdown\n");
        }
        else
        {
            dropdown_list_focusable->focused = 0;
            ((actor_focusable_T*)s_scene_editor->dropdown_list)->visible = 0;
            printf("Please dont show dropdown\n");
        }

        KEYBOARD_STATE->key_locks[GLFW_KEY_I] = 1;
    }

}

void scene_scene_editor_draw(scene_T* self)
{
    state_T* state = (state_T*) self;
    scene_scene_editor_T* s_scene_editor = (scene_scene_editor_T*) self;

    int number_of_scenes = 0;

    char scene_index_str[16];
    sprintf(scene_index_str, "%d / %d", (int)s_scene_editor->scene_index, number_of_scenes);

    draw_text(
        scene_index_str,
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

void scene_scene_editor_goto_next(scene_scene_editor_T* self)
{
    self->scene_index += 1;
}

void scene_scene_editor_goto_prev(scene_scene_editor_T* self)
{
    if (self->scene_index > 0)
        self->scene_index -= 1;

    //scene_scene_editor_refresh_grid(self);
}

void _scene_free(void* item)
{
    scene_free((scene_T*) item);
}

void scene_scene_editor_delete_current_scene(scene_scene_editor_T* self)
{
    // TODO: call SQL delete here
}
