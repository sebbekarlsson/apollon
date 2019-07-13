#include "include/dropdown_list.h"
#include <coelum/theatre.h>
#include <coelum/draw_utils.h>
#include <coelum/input.h>
#include "include/database.h"
#include <string.h>


extern theatre_T* THEATRE;
extern keyboard_state_T* KEYBOARD_STATE;

extern float COLOR_BG_DARK[3];
extern float COLOR_BG_DARK_BRIGHT[3];
extern float COLOR_FG[3];

dropdown_list_T* init_dropdown_list(float x, float y, float z, void (*press)(void* option))
{
    dropdown_list_T* dropdown_list = calloc(1, sizeof(struct DROPDOWN_LIST_STRUCT));
    actor_T* actor = (actor_T*) dropdown_list;
    actor_constructor(actor, x, y, z, dropdown_list_tick, dropdown_list_draw, "dropdown_list");
    
    dropdown_list->options = init_dynamic_list(sizeof(struct DROPDOWN_LIST_OPTION_STRUCT*));
    dropdown_list->option_index = 0;
    dropdown_list->expanded = 0;
    dropdown_list->visible = 1;
    dropdown_list->selected_index = 0;
    dropdown_list->width = 200;
    dropdown_list->press = press;

    return dropdown_list;
}

void dropdown_list_tick(actor_T* self)
{
    dropdown_list_T* dropdown_list = (dropdown_list_T*) self;
    actor_focusable_T* actor_focusable = (actor_focusable_T*) dropdown_list;

    if (!actor_focusable->focused)
    {
        dropdown_list->expanded = 0;
        return;
    }
    else
    {
        dropdown_list->expanded = 1;
    }

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
    
    if (KEYBOARD_STATE->keys[GLFW_KEY_ENTER] && !KEYBOARD_STATE->key_locks[GLFW_KEY_ENTER])
    {
        dropdown_list->selected_index = dropdown_list->option_index;
        KEYBOARD_STATE->key_locks[GLFW_KEY_ENTER] = 1;

        if (dropdown_list->press)
        {
            dropdown_list->press(dropdown_list->options->items[dropdown_list->option_index]);
        }
    }
}

void dropdown_list_option_draw(int i, dropdown_list_option_T* option, dropdown_list_T* dropdown_list, state_T* state)
{
    actor_focusable_T* actor_focusable = (actor_focusable_T*) dropdown_list;
    actor_T* self = (actor_T*) actor_focusable;

    if ((i == dropdown_list->option_index || i == dropdown_list->selected_index) && actor_focusable->focused)
    {
        draw_positioned_2D_mesh(
            self->x,
            (self->y + (i * 32)),
            0.0f,
            dropdown_list->width,
            32,
            COLOR_BG_DARK_BRIGHT[0],
            COLOR_BG_DARK_BRIGHT[1],
            COLOR_BG_DARK_BRIGHT[2],
            state
        );
    }

    int text_padding = 0;

    if (option->sprite)
    {
        draw_positioned_sprite(
            option->sprite,
            self->x + 4,
            (self->y + (i * 32)) + 8,
            0.0f,
            16,
            16,
            state
        );

        text_padding = 16;
    }

    if (option->key)
    {
        if (strlen(option->key))
        {
            draw_text(
                option->key,
                self->x + (32 / 2) + text_padding,
                (self->y + (i * 32)) + (32 / 2),
                0,
                COLOR_FG[0], // r
                COLOR_FG[1], // g
                COLOR_FG[2], // b
                6,
                6,
                option->text_limit,
                state
            );
        }
    }
}

void dropdown_list_draw(actor_T* self)
{
    dropdown_list_T* dropdown_list = (dropdown_list_T*) self;

    if (!dropdown_list->visible)
        return;

    scene_T* scene = scene_manager_get_current_scene(THEATRE->scene_manager);
    state_T* state = (state_T*) scene;

    draw_positioned_2D_mesh(
        self->x,
        self->y,
        0.0f,
        dropdown_list->width,
        32 * (dropdown_list->expanded ? dropdown_list->options->size : 1),
        COLOR_BG_DARK[0],
        COLOR_BG_DARK[1],
        COLOR_BG_DARK[2],
        state
    ); 

    if (dropdown_list->expanded)
    {
        for (int i = 0; i < dropdown_list->options->size; i++)
        {
            dropdown_list_option_T* option = (dropdown_list_option_T*) dropdown_list->options->items[i];

            dropdown_list_option_draw(i, option, dropdown_list, state);
        }
    }
    else
    {
        for (int i = 0; i < dropdown_list->options->size; i++)
        {
            dropdown_list_option_T* option = (dropdown_list_option_T*) dropdown_list->options->items[i];

            if (i == dropdown_list->selected_index)
            {
                dropdown_list_option_draw(0, option, dropdown_list, state);
                break;
            }
        }
    }
}

dropdown_list_option_T* init_dropdown_list_option(sprite_T* sprite, char* key, void* value, unsigned int text_limit)
{
    dropdown_list_option_T* option = calloc(1, sizeof(struct DROPDOWN_LIST_OPTION_STRUCT));
    option->sprite = sprite;
    option->key = key;
    option->value = value;
    option->text_limit = text_limit;

    return option;
}
