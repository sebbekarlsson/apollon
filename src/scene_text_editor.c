#include "include/scene_text_editor.h"
#include <coelum/constants.h>


scene_text_editor_T* init_scene_text_editor()
{
    scene_text_editor_T* s_text_editor = calloc(1, sizeof(struct SCENE_TEXT_EDITOR_STRUCT));
    scene_T* scene = (scene_T*) s_text_editor;
    scene_constructor(scene, scene_text_editor_tick, scene_text_editor_draw, 2);
    state_T* state = (state_T*) scene;

    s_text_editor->focus_manager = init_focus_manager();

    s_text_editor->textarea = init_textarea(0.0f, 0.0f, 0.0f, WINDOW_WIDTH, WINDOW_HEIGHT);
    dynamic_list_append(s_text_editor->focus_manager->focusables, s_text_editor->textarea);
    dynamic_list_append(state->actors, s_text_editor->textarea);

    return s_text_editor;
}

void scene_text_editor_tick(scene_T* self)
{
    scene_text_editor_T* s_text_editor = (scene_text_editor_T*) self;

    focus_manager_tick(s_text_editor->focus_manager);
}

void scene_text_editor_draw(scene_T* self)
{
}
