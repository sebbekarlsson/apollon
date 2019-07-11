#include "include/scene_actor_editor.h"
#include "include/etc.h"
#include <coelum/input.h>
#include <string.h>


extern keyboard_state_T* KEYBOARD_STATE;

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

    s_actor_editor->focus_manager = init_focus_manager();
    
    sprite_T* mock_sprite = init_sprite_from_file("res/img/tomato.png", GL_RGBA, 1, 16, 16);

    float margin = 64;
    float label_margin = 16; 


    /* ==== LEFT ==== */

    float ix = margin;
    float iy = margin;

    /* ==== actor ==== */
    s_actor_editor->label_actor = init_label(ix, iy, 0.0f, "Actor");
    iy += label_margin;
    s_actor_editor->dropdown_list_actor = init_dropdown_list(ix, iy, 0.0f);
    s_actor_editor->dropdown_list_actor->expanded = 0;
    dynamic_list_append(s_actor_editor->dropdown_list_actor->options, init_dropdown_list_option(mock_sprite, "item 0", (void*) 0));
    dynamic_list_append(s_actor_editor->dropdown_list_actor->options, init_dropdown_list_option(mock_sprite, "item 1", (void*) 0));
    dynamic_list_append(s_actor_editor->dropdown_list_actor->options, init_dropdown_list_option(mock_sprite, "item 2", (void*) 0));

    dynamic_list_append(s_actor_editor->focus_manager->focusables, (actor_focusable_T*) s_actor_editor->dropdown_list_actor);
    dynamic_list_append(state->actors, s_actor_editor->label_actor);
    dynamic_list_append(state->actors, s_actor_editor->dropdown_list_actor);
    iy += margin;

    s_actor_editor->button_new_actor = init_button(ix, iy, 0.0f, "x");
    dynamic_list_append(s_actor_editor->focus_manager->focusables, (actor_focusable_T*) s_actor_editor->button_new_actor);
    dynamic_list_append(state->actors, s_actor_editor->button_new_actor);


    /* ==== RIGHT ==== */
    
    float jx = 640 / 2;
    float jy = margin;

    /* ==== type_name ====*/
    s_actor_editor->label_type_name = init_label(jx, jy, 0.0f, "Name");
    jy += label_margin;
    s_actor_editor->input_field_type_name = init_input_field(jx, jy, 0.0f);
    dynamic_list_append(s_actor_editor->focus_manager->focusables, (actor_focusable_T*) s_actor_editor->input_field_type_name);
    dynamic_list_append(state->actors, s_actor_editor->label_type_name);
    dynamic_list_append(state->actors, s_actor_editor->input_field_type_name);
    jy += margin;

    
    /* ==== sprite ==== */
    s_actor_editor->label_sprite = init_label(jx, jy, 0.0f, "Sprite");
    jy += label_margin;
    s_actor_editor->dropdown_list_sprite = init_dropdown_list(jx, jy, 0.0f);
    s_actor_editor->dropdown_list_sprite->expanded = 0;
    ((actor_T*)s_actor_editor->dropdown_list_sprite)->z = 10;
    dynamic_list_append(s_actor_editor->dropdown_list_sprite->options, init_dropdown_list_option(mock_sprite, "item 0", (void*) 0));
    dynamic_list_append(s_actor_editor->dropdown_list_sprite->options, init_dropdown_list_option(mock_sprite, "item 1", (void*) 0));
    dynamic_list_append(s_actor_editor->dropdown_list_sprite->options, init_dropdown_list_option(mock_sprite, "item 2", (void*) 0));

    dynamic_list_append(s_actor_editor->focus_manager->focusables, (actor_focusable_T*) s_actor_editor->dropdown_list_sprite);
    dynamic_list_append(state->actors, s_actor_editor->label_sprite);
    dynamic_list_append(state->actors, s_actor_editor->dropdown_list_sprite);
    jy += margin;


    /* ==== tick_script ====*/
    s_actor_editor->label_tick_script = init_label(jx, jy, 0.0f, "Tick Script");
    jy += label_margin;
    s_actor_editor->input_field_tick_script = init_input_field(jx, jy, 0.0f);
    dynamic_list_append(s_actor_editor->focus_manager->focusables, (actor_focusable_T*) s_actor_editor->input_field_tick_script);
    dynamic_list_append(state->actors, s_actor_editor->label_tick_script);
    dynamic_list_append(state->actors, s_actor_editor->input_field_tick_script);
    jy += margin;

    /* ==== draw_script ====*/
    s_actor_editor->label_draw_script = init_label(jx, jy, 0.0f, "Draw Script");
    jy += label_margin;
    s_actor_editor->input_field_draw_script = init_input_field(jx, jy, 0.0f);
    dynamic_list_append(s_actor_editor->focus_manager->focusables, (actor_focusable_T*) s_actor_editor->input_field_draw_script);
    dynamic_list_append(state->actors, s_actor_editor->label_draw_script);
    dynamic_list_append(state->actors, s_actor_editor->input_field_draw_script);
    jy += margin;

    /* ==== save button ====*/
    s_actor_editor->button_save = init_button(jx, jy, 0.0f, "Save");
    dynamic_list_append(s_actor_editor->focus_manager->focusables, (actor_focusable_T*) s_actor_editor->button_save);
    dynamic_list_append(state->actors, s_actor_editor->button_save);

    state_resort_actors(state);

    return s_actor_editor;
}

void scene_actor_editor_tick(scene_T* self)
{
    go_back_on_escape();

    scene_actor_editor_T* s_actor_editor = (scene_actor_editor_T*) self;

    focus_manager_tick(s_actor_editor->focus_manager); 
}

void scene_actor_editor_draw(scene_T* self)
{
}
