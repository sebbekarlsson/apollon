#ifndef APOLLON_SCENE_SCENE_DESIGNER_H
#define APOLLON_SCENE_SCENE_DESIGNER_H
#include <coelum/scene.h>
#include <coelum/dynamic_list.h>
#include "dropdown_list.h"
#include "grid.h"
#include "actor_cursor.h"


typedef struct SCENE_SCENE_DESIGNER_STRUCT
{
    scene_T base;
    dropdown_list_T* dropdown_list;
    int scene_index;
    unsigned int scene_count;
    actor_cursor_T* actor_cursor;
    dynamic_list_T* database_actor_instances;
    database_scene_T* database_scene;
} scene_scene_designer_T;

scene_scene_designer_T* init_scene_scene_designer();

void scene_scene_designer_tick(scene_T* self);

void scene_scene_designer_draw(scene_T* self);

void scene_scene_designer_sync_database_actor_instances(scene_scene_designer_T* s_scene_designer);

void scene_scene_designer_empty_database_actor_instances(scene_scene_designer_T* s_scene_designer);

unsigned int scene_scene_designer_has_database_actor_instance_by_id(scene_scene_designer_T* s_scene_designer, const char* id);

void scene_scene_designer_refresh_state(scene_scene_designer_T* s_scene_designer);
#endif
