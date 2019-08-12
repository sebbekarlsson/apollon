#include "include/scene_base.h"
#include "include/main.h"
#include <string.h>
#include <coelum/constants.h>
#include <coelum/draw_utils.h>


extern main_state_T* MAIN_STATE;


scene_base_T* scene_base_constructor(scene_base_T* scene_base, void (*refresh_state)(struct SCENE_BASE_STRUCT* self), const char* title)
{
    scene_base->focus_manager = init_focus_manager();
    scene_base->refresh_state = refresh_state;
    scene_base->title = calloc(strlen(title) + 1, sizeof(char));
    strcpy(scene_base->title, title);

    return scene_base;
}

void scene_base_tick(scene_base_T* scene_base)
{
    focus_manager_tick(scene_base->focus_manager);

    if (MAIN_STATE->modal_is_active)
        focus_manager_keep_disabled(scene_base->focus_manager); 
}

void scene_base_draw(scene_base_T* scene_base)
{
    scene_base_draw_title_bar(scene_base);
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
}

actor_focusable_T* scene_base_register_focusable(scene_base_T* scene_base, actor_focusable_T* focusable)
{
    dynamic_list_append(scene_base->focus_manager->focusables, focusable);
    dynamic_list_append(((state_T*)((scene_T*)scene_base))->actors, (actor_T*)focusable);

    return focusable;
}
