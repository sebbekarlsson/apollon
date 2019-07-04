#include "include/dropdown_list.h"
#include <coelum/theatre.h>
#include <coelum/draw_utils.h>
#include <coelum/input.h>


extern theatre_T* THEATRE;
extern keyboard_state_T* KEYBOARD_STATE;

extern float COLOR_BG_DARK[3];
extern float COLOR_BG_DARK_BRIGHT[3];
extern float COLOR_FG[3];

dropdown_list_T* init_dropdown_list(float x, float y, float z)
{
    dropdown_list_T* dropdown_list = calloc(1, sizeof(struct DROPDOWN_LIST_STRUCT));
    actor_T* actor = (actor_T*) dropdown_list;
    actor_constructor(actor, x, y, z, dropdown_list_tick, dropdown_list_draw, "dropdown_list");

    dropdown_list->options = init_dynamic_list(sizeof(struct DROPDOWN_LIST_OPTION_STRUCT));
    dropdown_list->focused = 0;
    dropdown_list->option_index = 0;

    return dropdown_list;
}

void dropdown_list_tick(actor_T* self)
{
    dropdown_list_T* dropdown_list = (dropdown_list_T*) self;

    if (!dropdown_list->focused)
        return;

    if (KEYBOARD_STATE->keys[GLFW_KEY_UP] && !KEYBOARD_STATE->key_locks[GLFW_KEY_UP])
    {
        if (dropdown_list->option_index > 0)
            dropdown_list->option_index -= 1;

        KEYBOARD_STATE->key_locks[GLFW_KEY_UP] = 1;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_DOWN] && !KEYBOARD_STATE->key_locks[GLFW_KEY_DOWN])
    {
        if (dropdown_list->option_index < dropdown_list->options->size - 1)
            dropdown_list->option_index += 1;

        KEYBOARD_STATE->key_locks[GLFW_KEY_DOWN] = 1;
    }
}

void dropdown_list_draw(actor_T* self)
{
    dropdown_list_T* dropdown_list = (dropdown_list_T*) self;

    if (!dropdown_list->focused)
        return;

    scene_T* scene = scene_manager_get_current_scene(THEATRE->scene_manager);
    state_T* state = (state_T*) scene;

    draw_positioned_2D_mesh(
        self->x,
        self->y,
        0.0f,
        200,
        32 * dropdown_list->options->size,
        COLOR_BG_DARK[0],
        COLOR_BG_DARK[1],
        COLOR_BG_DARK[2],
        state
    );

    for (int i = 0; i < dropdown_list->options->size; i++)
    {
        dropdown_list_option_T* option = (dropdown_list_option_T*) dropdown_list->options->items[i];

        if (i == dropdown_list->option_index)
        {
            draw_positioned_2D_mesh(
                self->x,
                (self->y + (i * 32)),
                0.0f,
                200,
                32,
                COLOR_BG_DARK_BRIGHT[0],
                COLOR_BG_DARK_BRIGHT[1],
                COLOR_BG_DARK_BRIGHT[2],
                state
            );
        }

        draw_text(
            option->key,
            self->x + (32 / 2),
            (self->y + (i * 32)) + (32 / 2),
            0,
            COLOR_FG[0], // r
            COLOR_FG[1], // g
            COLOR_FG[2], // b
            6,
            6,
            state
        );
    }
}

dropdown_list_option_T* init_dropdown_list_option(char* key, void* value)
{
    dropdown_list_option_T* option = calloc(1, sizeof(struct DROPDOWN_LIST_OPTION_STRUCT));
    option->key = key;
    option->value = value;

    return option;
}
