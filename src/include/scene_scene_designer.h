#ifndef APOLLON_SCENE_SCENE_DESIGNER_H
#define APOLLON_SCENE_SCENE_DESIGNER_H
#include <coelum/scene.h>
#include "dropdown_list.h"
#include "grid.h"


typedef struct SCENE_SCENE_DESIGNER_STRUCT
{
    scene_T base;
    dropdown_list_T* dropdown_list;
    int scene_index;
    unsigned int scene_count;
    char* scene_id; // the scene which is currenlty being modified.
} scene_scene_designer_T;

scene_scene_designer_T* init_scene_scene_designer();

void scene_scene_designer_tick(scene_T* self);

void scene_scene_designer_draw(scene_T* self);
#endif
