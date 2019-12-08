#include "include/scene_text_editor.h"
#include "include/etc.h"
#include "include/main.h"
#include <coelum/constants.h>
#include <coelum/draw_utils.h>
#include <string.h>


extern main_state_T* MAIN_STATE;
extern const float COLOR_FG[3];


static void scene_text_editor_load(void* s)
{
    scene_T* self = (scene_T*) s;
    scene_text_editor_T* s_text_editor = (scene_text_editor_T*) self;

    if (MAIN_STATE->script_id != (void*)0 && MAIN_STATE->text_editor_value != (void*)0)
        ((component_textable_T*)s_text_editor->component_textarea)->value = MAIN_STATE->text_editor_value;
}

scene_text_editor_T* init_scene_text_editor()
{
    scene_text_editor_T* s_text_editor = calloc(1, sizeof(struct SCENE_TEXT_EDITOR_STRUCT));
    scene_T* scene = (scene_T*) s_text_editor;
    scene_constructor(scene, scene_text_editor_tick, scene_text_editor_draw, 2);
    scene->load = scene_text_editor_load;
    state_T* state = (state_T*) scene;

    scene->type_name = "text_editor";

    s_text_editor->focus_manager = init_focus_manager();

    s_text_editor->component_textarea = init_component_textarea(s_text_editor->focus_manager, 0.0f, 0.0f, RES_WIDTH, RES_HEIGHT);
    dynamic_list_append(state->actors, s_text_editor->component_textarea);
    dynamic_list_append(s_text_editor->focus_manager->components, (actor_component_T*) s_text_editor->component_textarea);

    s_text_editor->line_bar_width = 24;

    return s_text_editor;
}

void scene_text_editor_tick(scene_T* self)
{
    state_T* state = (state_T*) self;
    scene_text_editor_T* s_text_editor = (scene_text_editor_T*) self;

    go_back_on_escape();

    component_textable_T* component_textable = (component_textable_T*) s_text_editor->component_textarea;
    component_textable->width = RES_WIDTH - s_text_editor->line_bar_width;
    ((actor_T*)component_textable)->x = s_text_editor->line_bar_width;

    if (component_textable->caret_y >= state->camera->y + RES_HEIGHT)
        state->camera->y += 16; // font_size + font_spacing

    if (component_textable->caret_y < state->camera->y)
        state->camera->y -= 16;

    focus_manager_tick(s_text_editor->focus_manager);

    ((actor_T*)component_textable)->height = RES_HEIGHT + state->camera->y;

    MAIN_STATE->text_editor_value = ((component_textable_T*)s_text_editor->component_textarea)->value;
}

void scene_text_editor_draw(scene_T* self)
{
    state_T* state = (state_T*) self;
    scene_text_editor_T* s_text_editor = (scene_text_editor_T*) self;

    camera_bind(state->camera);

    draw_positioned_2D_mesh(
        0.0f,
        0.0f,
        0.0f,
        s_text_editor->line_bar_width,
        state->camera->y + RES_HEIGHT,
        0, 0, 0,
        1.0f,
        state
    );
    
    char* text = calloc(256, sizeof(char));

    for (int i = 0; i < component_textable_get_number_of_lines((component_textable_T*) s_text_editor->component_textarea)+1; i++)
    {
        sprintf(text, "%d", i);
        s_text_editor->line_bar_width = strlen(text) * (4 + 4);

        draw_text(
            text,
            8,
            4 + (i * (4 + 4)),
            ((actor_T*)s_text_editor->component_textarea)->z + 0.1f,
            COLOR_FG[0],
            COLOR_FG[1],
            COLOR_FG[2],
            1.0f, // a
            4,
            4,
            0,
            state
        );
    }

    free(text);

    camera_unbind(state->camera);
}
