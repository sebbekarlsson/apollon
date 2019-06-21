#include "include/scene_sprite_editor.h"
#include <coelum/constants.h>
#include <coelum/actor_text.h>
#include <string.h>


scene_sprite_editor_T* init_scene_sprite_editor()
{
    scene_sprite_editor_T* s_sprite_editor = calloc(1, sizeof(struct SCENE_SPRITE_EDITOR_STRUCT));
    scene_T* s = (scene_T*) s_sprite_editor;
    state_T* state = (state_T*) s; 

    scene_constructor(s, scene_sprite_editor_tick, scene_sprite_editor_draw, 2);

    s->type_name = "sprite_editor";
    s->bg_r = 255;
    s->bg_g = 255;
    s->bg_b = 255;

    s_sprite_editor->grid = init_grid(
        0.0f,
        0.0f,
        0.0f,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        16,
        0,
        0,
        0       
    );

    dynamic_list_append(state->actors, s_sprite_editor->grid);

    return s_sprite_editor;
}

void scene_sprite_editor_tick(scene_T* self)
{
}

void scene_sprite_editor_draw(scene_T* self)
{
}
