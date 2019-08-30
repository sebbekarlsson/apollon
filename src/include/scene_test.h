#ifndef APOLLON_SCENE_TEST
#define APOLLON_SCENE_TEST
#include <coelum/dynamic_list.h>
#include "scene_base.h"


typedef struct SCENE_TEST_STRUCT
{
    scene_base_T base;
} scene_test_T;

scene_test_T* init_scene_test();

void scene_test_tick(scene_T* self);

void scene_test_draw(scene_T* self);

void scene_test_refresh_state(scene_base_T* scene_base);
#endif
