#include "include/scene_text_editor.h"
#include <coelum/constants.h>
#include <coelum/draw_utils.h>
#include <string.h>


scene_text_editor_T* init_scene_text_editor()
{
    scene_text_editor_T* s_text_editor = calloc(1, sizeof(struct SCENE_TEXT_EDITOR_STRUCT));
    scene_T* scene = (scene_T*) s_text_editor;
    scene_constructor(scene, scene_text_editor_tick, scene_text_editor_draw, 2);
    state_T* state = (state_T*) scene;

    scene->type_name = "text_editor";

    s_text_editor->focus_manager = init_focus_manager();

    s_text_editor->textarea = init_textarea(0.0f, 0.0f, WINDOW_WIDTH, WINDOW_HEIGHT);
    dynamic_list_append(s_text_editor->focus_manager->focusables, s_text_editor->textarea);
    dynamic_list_append(state->actors, s_text_editor->textarea);

    s_text_editor->line_bar_width = 24;

    return s_text_editor;
}

void scene_text_editor_tick(scene_T* self)
{
    scene_text_editor_T* s_text_editor = (scene_text_editor_T*) self;

    actor_textable_T* actor_textable = (actor_textable_T*) s_text_editor->textarea;
    actor_textable->width = WINDOW_WIDTH - s_text_editor->line_bar_width;
    ((actor_T*)actor_textable)->x = s_text_editor->line_bar_width;

    focus_manager_tick(s_text_editor->focus_manager); 
}

void scene_text_editor_draw(scene_T* self)
{
    state_T* state = (state_T*) self;
    scene_text_editor_T* s_text_editor = (scene_text_editor_T*) self;

    draw_positioned_2D_mesh(
        0.0f,
        0.0f,
        0.0f,
        s_text_editor->line_bar_width,
        WINDOW_HEIGHT,
        0, 0, 0,
        1.0f,
        state
    );
    
    char* text = calloc(256, sizeof(char));

    for (int i = 0; i < actor_textable_get_number_of_lines((actor_textable_T*) s_text_editor->textarea)+1; i++)
    {
        sprintf(text, "%d", i);
        s_text_editor->line_bar_width = strlen(text) * (8 + 8);

        draw_text(
            text,
            8,
            12 + (i * (8 + 8)),
            0,
            255,
            255,
            255,
            1.0f, // a
            8,
            8,
            0,
            state
        );
    }

    free(text);
}
