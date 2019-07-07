#include "include/scene_actor_editor.h"
#include "include/etc.h"


scene_actor_editor_T* init_scene_actor_editor()
{
    scene_actor_editor_T* s_actor_editor = calloc(1, sizeof(struct SCENE_ACTOR_EDITOR_STRUCT));
    scene_T* scene = (scene_T*) s_actor_editor;
    state_T* state = (state_T*) scene;
    scene_constructor(scene, scene_actor_editor_tick, scene_actor_editor_draw, 2);

    scene->type_name = "actor_editor";

    scene->bg_r = 255;
    scene->bg_g = 255;
    scene->bg_b = 255;

    s_actor_editor->test_input = init_input_field(120, 120, 0.0f);
    s_actor_editor->test_input->value = "hello world";
    s_actor_editor->test_input->focused = 1;

    dynamic_list_append(state->actors, s_actor_editor->test_input);

    return s_actor_editor;
}

void scene_actor_editor_tick(scene_T* self)
{
    go_back_on_escape();
}

void scene_actor_editor_draw(scene_T* self)
{
}
