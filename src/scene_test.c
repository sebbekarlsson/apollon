#include "include/scene_test.h"
#include "include/component_button.h"


void button_press() {}

scene_test_T* init_scene_test()
{
    scene_test_T* scene_test = calloc(1, sizeof(struct SCENE_TEST_STRUCT));
    scene_base_T* scene_base = (scene_base_T*) scene_test;
    scene_T* scene = (scene_T*) scene_base_constructor(scene_base, scene_test_refresh_state, "Test");
    scene_constructor(scene, scene_test_tick, scene_test_draw, 2);

    for (int i = 0; i < 10; i++)
    {
        component_pane_add_component(
            scene_base->component_pane,
                (actor_component_T*) init_component_button(0.0f, 0.0f, 0.0f, "Test", button_press) 
        );
    }

    return scene_test;
}

void scene_test_tick(scene_T* self)
{
    scene_base_T* scene_base = (scene_base_T*) self;
    scene_base_tick(scene_base); 
}

void scene_test_draw(scene_T* self)
{
    scene_base_T* scene_base = (scene_base_T*) self;
    scene_base_draw(scene_base);

    state_T* s = (state_T*) ((scene_T*)scene_base->component_pane);
}

void scene_test_refresh_state(scene_base_T* scene_base)
{
}
