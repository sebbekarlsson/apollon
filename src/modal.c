#include "include/modal.h"
#include "include/modal_manager.h"
#include <coelum/current.h>
#include <coelum/draw_utils.h>
#include <coelum/constants.h>
#include <string.h>


extern const float COLOR_GRAY[3];
extern const float COLOR_BG_DARK_BRIGHT[3];

extern modal_manager_T* MODAL_MANAGER;

void modal_button_press()
{
    printf("Pressed modal button!\n");
    modal_manager_close_all_modals(MODAL_MANAGER);
}

modal_T* init_modal(float x, float y, char* title, char* text, state_T* state)
{
    modal_T* modal = calloc(1, sizeof(struct MODAL_STRUCT));
    actor_T* actor = (actor_T*) modal;
    actor_constructor(actor, x, y, 10.0f, modal_tick, modal_draw, "modal");

    modal->title = title;
    modal->text = text;
    modal->width = 400;
    modal->height = 256;
    modal->focus_manager = init_focus_manager();

    actor->x -= (modal->width / 2);
    actor->y -= (modal->height / 2);

    modal->button = init_button(
        actor->x + 100,
        (actor->y + modal->height) - 64,
        11.0f,
        "OK",
        modal_button_press
    );
    
    dynamic_list_append(state->actors, modal->button);
    //dynamic_list_append(modal->focus_manager->focusables, modal->button);

    return modal;
}

void modal_tick(actor_T* self)
{ 
    modal_T* modal = (modal_T*) self;
    focus_manager_tick(modal->focus_manager);
}

void modal_draw(actor_T* self)
{
    modal_T* modal = (modal_T*) self;

    scene_T* scene = get_current_scene();
    state_T* state = (state_T*) scene;

    unsigned int bar_height = 32; 
   
    // TODO: fix the z-buffer when doing 2D, so that the modal_manager
    // can draw this on its own. 
    modal_manager_draw(MODAL_MANAGER);

    // shadow
    draw_positioned_2D_mesh(
        self->x + 4,
        self->y + 4,
        0.0f,
        modal->width,
        modal->height,
        0,
        0,
        0,
        0.6f,
        state
    );

    draw_positioned_2D_mesh(
        self->x,
        self->y,
        self->z,
        modal->width,
        modal->height,
        COLOR_GRAY[0],
        COLOR_GRAY[1],
        COLOR_GRAY[2],
        1,
        state
    );

    draw_positioned_2D_mesh(
        self->x,
        self->y,
        self->z,
        modal->width,
        bar_height,
        COLOR_BG_DARK_BRIGHT[0],
        COLOR_BG_DARK_BRIGHT[1],
        COLOR_BG_DARK_BRIGHT[2],
        1,
        state
    );

    draw_text(
        modal->title,
        (self->x + (modal->width / 2)) - ((strlen(modal->title) * (6 + 6)) / 2),
        self->y + (bar_height / 2),
        0.0f,
        255,
        255,
        255,
        1.0f, // a
        6,
        6,
        0,
        state
    );

    draw_text(
        modal->text,
        (self->x + (modal->width / 2)) - ((strlen(modal->text) * (6 + 6)) / 2),
        (self->y + ((modal->height + bar_height) / 2)) - 32,
        0.0f,
        255,
        255,
        255,
        1.0f, // a
        6,
        6,
        0,
        state
    );
}

void modal_free(modal_T* modal)
{
    //free(modal->title);
    //free(modal->text);
    button_free(modal->button);
    focus_manager_free(modal->focus_manager);

    actor_T* actor = (actor_T*) modal;

    actor_free(actor);

    free(modal);
}
