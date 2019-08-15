#include "include/select_list.h"
#include <coelum/scene.h>
#include <coelum/actor.h>
#include <coelum/actor_text.h>
#include <coelum/constants.h>
#include <coelum/theatre.h>
#include <coelum/input.h>
#include <string.h>



extern float COLOR_BG_DARK_BRIGHT[3];
extern float COLOR_FG[3];

extern keyboard_state_T* KEYBOARD_STATE;

select_list_T* init_select_list(state_T* state)
{
    select_list_T* select_list = calloc(1, sizeof(struct SELECT_LIST_STRUCT));

    select_list->options = init_dynamic_list(sizeof(struct SCENE_MENU_OPTION_STRUCT*));
    select_list->option_index = 0;
    select_list->state = state;

    return select_list;
}

select_list_option_T* select_list_register_option(select_list_T* select_list, char* name, void (*callback)())
{
    state_T* state = select_list->state;
    select_list_option_T* menu_option = calloc(1, sizeof(struct SCENE_MENU_OPTION_STRUCT));
    menu_option->name = name;
    menu_option->callback = callback;

    dynamic_list_append(select_list->options, menu_option);

    float text_size = 15;
    float text_spacing = 10;
    float text_y_spacing = 44;

    actor_text_T* actor_text = init_actor_text(
        (WINDOW_WIDTH / 2) - (((strlen(name)-1) * (text_size + text_spacing)) / 2),
        select_list->options->size * text_y_spacing,
        0.0f,
        name,
        COLOR_BG_DARK_BRIGHT[1],
        COLOR_BG_DARK_BRIGHT[2],
        COLOR_BG_DARK_BRIGHT[3],
        text_size,
        text_spacing
    );

    dynamic_list_append(state->actors, actor_text);

    unsigned int nr_texts = 0;

    // count number of text_actors
    for (int i = 0; i < state->actors->size; i++)
    {
        actor_T* actor = (actor_T*) state->actors->items[i];

        if (strcmp(actor->type_name, "text") == 0)
        {
            nr_texts += 1;
        }
    }

    // center everything
    for (int i = 0; i < state->actors->size; i++)
    {
        actor_T* actor = (actor_T*) state->actors->items[i];

        if (strcmp(actor->type_name, "text") == 0)
        {
            actor->y = (text_y_spacing / 2) + (((WINDOW_HEIGHT / 2) - ((nr_texts * text_y_spacing) / 2) + (i*text_y_spacing)));

            actor->y += 64;
        }
    }

    return menu_option;
}

void select_list_tick(select_list_T* select_list)
{
    state_T* state = select_list->state;

    if (KEYBOARD_STATE->keys[GLFW_KEY_DOWN] && !KEYBOARD_STATE->key_locks[GLFW_KEY_DOWN])
    {
        if (select_list->option_index < select_list->options->size - 1)
        {
            select_list->option_index += 1;
        }
        else
        {
            select_list->option_index = 0;
        }

        KEYBOARD_STATE->key_locks[GLFW_KEY_DOWN] = 1;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_UP] && !KEYBOARD_STATE->key_locks[GLFW_KEY_UP])
    {
        if (select_list->option_index > 0)
        {
            select_list->option_index -= 1;
        }
        else
        {
            select_list->option_index = select_list->options->size - 1;
        }

        KEYBOARD_STATE->key_locks[GLFW_KEY_UP] = 1;
    }

    select_list_option_T* current_option = (select_list_option_T*) select_list->options->items[select_list->option_index];

    if (KEYBOARD_STATE->keys[GLFW_KEY_ENTER] && current_option->callback)
    {
        current_option->callback();
    }

    for (int i = 0; i < state->actors->size; i++)
    {
        actor_T* actor = (actor_T*) state->actors->items[i];

        if (strcmp(actor->type_name, "text") == 0)
        {
            actor_text_T* actor_text = (actor_text_T*) actor;
            
            if (i == select_list->option_index)
            {
                actor_text->r = COLOR_FG[0];
                actor_text->g = COLOR_FG[1];
                actor_text->b = COLOR_FG[2];
            }
            else
            {
                actor_text->r = COLOR_BG_DARK_BRIGHT[0];
                actor_text->g = COLOR_BG_DARK_BRIGHT[1];
                actor_text->b = COLOR_BG_DARK_BRIGHT[2];
            }
        }
    }
}
