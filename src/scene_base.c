#include "include/main.h"
#include "include/scene_base.h"
#include "include/modal_manager.h"
#include <string.h>
#include <coelum/theatre.h>
#include <coelum/constants.h>
#include <coelum/draw_utils.h>


extern theatre_T* THEATRE;
extern main_state_T* MAIN_STATE;
extern modal_manager_T* MODAL_MANAGER;

extern sprite_T* SPRITE_BACK;


static void component_button_back_press(actor_T* self)
{
    if (THEATRE->scene_manager->scene_index <= 0)
        return;
  
    scene_manager_go_back(THEATRE->scene_manager);
}

scene_base_T* scene_base_constructor(scene_base_T* scene_base, void (*refresh_state)(struct SCENE_BASE_STRUCT* self), const char* title)
{
    scene_base->focus_manager = init_focus_manager();
    scene_base->refresh_state = refresh_state;
    scene_base->title = calloc(strlen(title) + 1, sizeof(char));
    strcpy(scene_base->title, title);

    state_T* state = (state_T*)((scene_T*) scene_base);

    scene_base->component_pane = init_component_pane(
      state,
      scene_base->focus_manager,
      0.0f,
      0.0f,
      WINDOW_WIDTH,
      WINDOW_HEIGHT - (24)
    );
    scene_base->component_pane->y += 24;

    scene_base->component_pane_title_bar = init_component_pane(
      state,
      scene_base->focus_manager,
      0.0f,
      0.0f,
      WINDOW_WIDTH,
      24
    );
    scene_base->component_pane_title_bar->z = 2.6f;
    scene_base->component_pane_title_bar->bg_visible = 0;
    scene_base->component_button_back = init_component_button(
        scene_base->focus_manager, 0, 0, 0, (void*)0, component_button_back_press
    );
    scene_base->component_button_back->sprite = SPRITE_BACK;
    scene_base->component_button_back->only_sprite = 1;
    component_pane_add_component(
        scene_base->component_pane_title_bar,
        (actor_component_T*) scene_base->component_button_back
    );

    return scene_base;
}

void scene_base_tick(scene_base_T* scene_base)
{
    ((actor_component_T*)scene_base->component_button_back)->visible = THEATRE->scene_manager->scene_index >= 1;
    ((scene_T*)scene_base->component_pane_title_bar)->tick((scene_T*)scene_base->component_pane_title_bar);
    state_T* title_bar_state = (state_T*)((scene_T*)scene_base->component_pane_title_bar);

    state_tick(title_bar_state);

    ((scene_T*)scene_base->component_pane)->tick((scene_T*)scene_base->component_pane);
    state_T* state = (state_T*)((scene_T*)scene_base->component_pane);

    state_tick(state);

    focus_manager_tick(scene_base->focus_manager);

    if (MAIN_STATE->modal_is_active)
        focus_manager_keep_disabled(scene_base->focus_manager); 
}

void scene_base_draw(scene_base_T* scene_base)
{
    state_T* s = (state_T*) ((scene_T*)scene_base->component_pane);
    state_T* state = (state_T*)((scene_T*)scene_base->component_pane);

    scene_base_draw_title_bar(scene_base);

    glEnable(GL_SCISSOR_TEST);
    glScissor((int)scene_base->component_pane->x, (int)(WINDOW_HEIGHT - scene_base->component_pane->y - scene_base->component_pane->height), (int)scene_base->component_pane->width, (int)scene_base->component_pane->height); 
    
    ((scene_T*)scene_base->component_pane)->draw((scene_T*)scene_base->component_pane);
    state_draw(state);

    glDisable(GL_SCISSOR_TEST);

    modal_manager_draw(MODAL_MANAGER);
}

void scene_base_draw_title_bar(scene_base_T* scene_base)
{
    state_T* state = (state_T*) scene_base;
    char* text = scene_base->title;
    
    if (text == (void*)0)
        return;

    int font_size = 6;
    int font_spacing = 6;
    int text_width = (strlen(text) - 1) * (font_size + font_spacing);
    float padding = 2 * (font_size + font_spacing);

    for (int i = 0; i < 6; i++)
    {
       float endx = ((WINDOW_WIDTH / 2) - (text_width / 2)) - padding;

       draw_line(
           0,
           i * 4,
           0,
           endx,
           i * 4,
           0,
           48,
           48,
           48,
           state
        );

        draw_line(
           padding + (WINDOW_WIDTH / 2) + (text_width / 2),
           i * 4,
           0,
           WINDOW_WIDTH,
           i * 4,
           0,
           48,
           48,
           48,
           state
        ); 
    }

    if (text != (void*) 0)
    {
        draw_text(
            text,
            (WINDOW_WIDTH / 2) - (text_width / 2),
            font_size + font_size,
            0,
            48,
            48,
            48,
            1.0f,
            font_size,
            font_spacing,
            0,
            state
        );
    }
    
    state_T* title_bar_state = (state_T*)((scene_T*)scene_base->component_pane_title_bar);

    glEnable(GL_SCISSOR_TEST);
    glScissor((int)scene_base->component_pane_title_bar->x, (int)(WINDOW_HEIGHT - scene_base->component_pane_title_bar->y - scene_base->component_pane_title_bar->height), (int)scene_base->component_pane_title_bar->width, (int)scene_base->component_pane_title_bar->height); 
    
    ((scene_T*)scene_base->component_pane_title_bar)->draw((scene_T*)scene_base->component_pane_title_bar);
    state_draw(title_bar_state);

    glDisable(GL_SCISSOR_TEST);
}
