#include "include/modal.h"
#include "include/main.h"
#include "include/modal_manager.h"
#include "include/component_label.h"
#include "include/string_utils.h"
#include <coelum/current.h>
#include <coelum/draw_utils.h>
#include <coelum/constants.h>
#include <string.h>


extern const float COLOR_GRAY[3];
extern const float COLOR_BG_DARK_BRIGHT[3];

extern modal_manager_T* MODAL_MANAGER;

#define MODAL_BAR_HEIGHT 32


static void modal_click(component_button_T* self)
{
    modal_manager_close_all_modals(MODAL_MANAGER);
}

modal_T* init_modal(float x, float y, char* title, char* text, state_T* state, focus_manager_T* focus_manager)
{
    modal_T* modal = calloc(1, sizeof(struct MODAL_STRUCT));
    actor_T* actor = (actor_T*) modal;
    actor_constructor(actor, x, y, MODAL_MANAGER->z + 0.1f, modal_tick, modal_draw, "modal");

    modal->title = string_copy(title);
    modal->text = string_copy(text);
    modal->width = 400;
    modal->height = 256;
    modal->component_pane = init_component_pane(
        state, focus_manager, x - modal->width/2, (y - modal->height / 2) + MODAL_BAR_HEIGHT, modal->width, modal->height - MODAL_BAR_HEIGHT
    );
    modal->component_pane->bg_visible = 0;
    modal->component_pane->z = actor->z + 0.6f;
    modal->component_pane->centered = 1;
    modal->component_pane->child_margin_top = 16;
    modal->component_button = init_component_button(
        focus_manager, x, y, actor->z + 0.7f, "OK", modal_click        
    );
    component_pane_add_component(modal->component_pane, (actor_component_T*) init_component_label(focus_manager, 0.0f, 0.0f, 0.0f, title, 255, 255, 255));
    component_pane_add_component(modal->component_pane, (actor_component_T*) init_component_label(focus_manager, 0.0f, 0.0f, 0.0f, text, 255, 255, 255));
    component_pane_add_component(modal->component_pane, (actor_component_T*) modal->component_button);

    actor->x -= (modal->width / 2);
    actor->y -= (modal->height / 2);

    return modal;
}

void modal_tick(actor_T* self)
{
    modal_T* modal = (modal_T*) self;
    ((scene_T*)modal->component_pane)->tick((scene_T*)modal->component_pane);
    state_T* modal_state = (state_T*)((scene_T*)modal->component_pane);

    state_tick(modal_state);
}

void modal_draw(actor_T* self)
{
    modal_T* modal = (modal_T*) self;
    state_T* modal_state = (state_T*)((scene_T*)modal->component_pane);

    scene_T* scene = get_current_scene();
    state_T* state = (state_T*) scene; 

    // shadow
    draw_positioned_2D_mesh(
        self->x + 4,
        self->y + 4,
        self->z,
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
        self->z + 0.1f,
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
        self->z + 0.2f,
        modal->width,
        MODAL_BAR_HEIGHT,
        COLOR_BG_DARK_BRIGHT[0],
        COLOR_BG_DARK_BRIGHT[1],
        COLOR_BG_DARK_BRIGHT[2],
        1,
        state
    );

    glEnable(GL_SCISSOR_TEST);
    glScissor((int)modal->component_pane->x, (int)(RES_HEIGHT - modal->component_pane->y - modal->component_pane->height), (int)modal->component_pane->width, (int)modal->component_pane->height); 
    
    ((scene_T*)modal->component_pane)->draw((scene_T*)modal->component_pane);
    state_draw(modal_state);

    glDisable(GL_SCISSOR_TEST);
}

void modal_free(modal_T* modal)
{
    //actor_T* actor = (actor_T*) modal;

    //actor_free(actor);

    free(modal->title);
    free(modal->text);

    //free(modal);
}
