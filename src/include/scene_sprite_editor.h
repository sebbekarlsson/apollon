#ifndef APOLLON_SCENE_SPRITE_EDITOR_H
#define APOLLON_SCENE_SPRITE_EDITOR_H
#include <coelum/scene.h>
#include <coelum/dynamic_list.h>
#include "grid.h"
#include "focus_manager.h"
#include "dropdown_list.h"
#include "input_field.h"
#include "label.h"
#include "button.h"


typedef struct SCENE_SPRITE_EDITOR_STRUCT
{
    scene_T base;
    grid_T* grid;
    grid_T* grid_color_selector;
    grid_T* grid_color_mixer;
    dynamic_list_T* grids;  // sprite frames
    label_T* label_current_sprite;
    dropdown_list_T* dropdown_list_sprite;
    label_T* label_name;
    input_field_T* input_field_name;
    button_T* button_save;
    button_T* button_new;
    unsigned int grid_index;
    char* sprite_id; // the sprite currently being modified
    float r;
    float g;
    float b;
    focus_manager_T* focus_manager;
} scene_sprite_editor_T;

scene_sprite_editor_T* init_scene_sprite_editor();

void scene_sprite_editor_tick(scene_T* self);

void scene_sprite_editor_draw(scene_T* self);

void scene_sprite_editor_goto_next(scene_sprite_editor_T* self);

void scene_sprite_editor_goto_prev(scene_sprite_editor_T* self);

void scene_sprite_editor_delete_current_frame(scene_sprite_editor_T* self);

void scene_sprite_editor_clear_all_frames(scene_sprite_editor_T* self);

void scene_sprite_editor_refresh_grid(scene_sprite_editor_T* self);

dynamic_list_T* scene_sprite_editor_get_frames_as_textures(scene_sprite_editor_T* self);
#endif
