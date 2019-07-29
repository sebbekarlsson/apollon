#include "include/modal_manager.h"
#include "include/main.h"
#include <coelum/current.h>
#include <coelum/constants.h>
#include <coelum/draw_utils.h>
#include <string.h>


extern main_state_T* MAIN_STATE;


modal_manager_T* init_modal_manager()
{
    modal_manager_T* modal_manager = calloc(1, sizeof(struct MODAL_MANAGER_STRUCT));
    modal_manager->modals = init_dynamic_list(sizeof(struct MODAL_STRUCT*));

    return modal_manager;
}

void modal_manager_draw(modal_manager_T* modal_manager)
{
    scene_T* scene = get_current_scene();
    state_T* state = (state_T*) scene;

    if (MAIN_STATE->modal_is_active && modal_manager->modals->size > 0)
    {
        // dim background
        draw_positioned_2D_mesh(
            0.0f,
            0.0f,
            0.0f,
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            255,
            255,
            255,
            0.6f,
            state
        );
    }
}

void modal_manager_register_modal(modal_manager_T* modal_manager, modal_T* modal)
{
    dynamic_list_append(modal_manager->modals, modal);
}

modal_T* modal_manager_show_modal(modal_manager_T* modal_manager, const char* title, const char* text)
{
    scene_T* scene = get_current_scene();
    state_T* state = (state_T*) scene;

    modal_T* modal = init_modal(
        WINDOW_WIDTH / 2,
        WINDOW_HEIGHT / 2,
        title,
        text,
        state
    );

    modal_manager_register_modal(modal_manager, modal);
    dynamic_list_append(state->actors, modal);

    MAIN_STATE->modal_is_active = 1;

    return modal;
}

void modal_manager_close_modal(modal_manager_T* modal_manager, modal_T* modal)
{
    scene_T* scene = get_current_scene();
    state_T* state = (state_T*) scene;

    dynamic_list_remove(modal_manager->modals, modal, (void*) 0);
    dynamic_list_remove(state->actors, modal, (void*) 0);
    dynamic_list_remove(state->actors, modal->button, (void*) 0);


    modal_free(modal);
}

void modal_manager_close_all_modals(modal_manager_T* modal_manager)
{
    for (int i = modal_manager->modals->size; i > 0; i--)
    {
        modal_T* modal = (modal_T*) modal_manager->modals->items[i - 1];
        modal_manager_close_modal(modal_manager, modal);
    }

    MAIN_STATE->modal_is_active = 0;
}
