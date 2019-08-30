#ifndef APOLLON_SCENE_SPRITE_EDITOR_H
#define APOLLON_SCENE_SPRITE_EDITOR_H
#include "scene_base.h"
#include <coelum/dynamic_list.h>
#include <athena/database.h>
#include "component_grid.h"
#include "component_dropdown_list.h"
#include "component_input_field.h"
#include "component_label.h"
#include "component_button.h"


typedef struct SCENE_SPRITE_EDITOR_STRUCT
{
    scene_base_T base;
    component_grid_T* component_grid;
    component_grid_T* component_grid_color_selector;
    component_grid_T* component_grid_tool_selector;
    component_grid_T* component_grid_color_mixer;
    dynamic_list_T* component_grids;  // sprite frames
    component_label_T* component_label_current_sprite;
    component_dropdown_list_T* component_dropdown_list_sprite;
    component_label_T* component_label_frame_delay;
    component_input_field_T* component_input_field_frame_delay;
    component_label_T* component_label_name;
    component_input_field_T* component_input_field_name;
    component_button_T* component_button_save;
    component_button_T* component_button_new;
    component_button_T* component_button_delete;
    component_label_T* component_label_frame_index;
    unsigned int component_grid_index;
    unsigned int tool_index;
    char* sprite_id; // the sprite currently being modified
    char* frame_index;
    database_sprite_T* current_database_sprite; // a copy of the current sprite being edited. Only for reading.
    float r;
    float g;
    float b;
    float a;
} scene_sprite_editor_T;

scene_sprite_editor_T* init_scene_sprite_editor();

void scene_sprite_editor_tick(scene_T* self);

void scene_sprite_editor_draw(scene_T* self);

void scene_sprite_editor_goto_next(scene_sprite_editor_T* self);

void scene_sprite_editor_goto_prev(scene_sprite_editor_T* self);

void scene_sprite_editor_delete_current_frame(scene_sprite_editor_T* self);

void scene_sprite_editor_clear_all_frames(scene_sprite_editor_T* self);

void scene_sprite_editor_refresh_state(scene_base_T* scene_base);

void scene_sprite_editor_refresh_component_grid(scene_sprite_editor_T* self);

void scene_sprite_editor_load_component_grids_from_sprite(scene_sprite_editor_T* s_sprite_editor, sprite_T* sprite);

dynamic_list_T* scene_sprite_editor_get_frames_as_textures(scene_sprite_editor_T* self);
#endif
