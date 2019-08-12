#include "include/scene_base.h"
#include "include/main.h"


extern main_state_T* MAIN_STATE;


scene_base_T* scene_base_constructor(scene_base_T* scene_base, void (*refresh_state)(struct SCENE_BASE_STRUCT* self))
{
    scene_base->focus_manager = init_focus_manager();
    scene_base->refresh_state = refresh_state;

    return scene_base;
}

void scene_base_tick(scene_base_T* scene_base)
{
    focus_manager_tick(scene_base->focus_manager);

    if (MAIN_STATE->modal_is_active)
        focus_manager_keep_disabled(scene_base->focus_manager); 
}

actor_focusable_T* scene_base_register_focusable(scene_base_T* scene_base, actor_focusable_T* focusable)
{
    dynamic_list_append(scene_base->focus_manager->focusables, focusable);
    dynamic_list_append(((state_T*)((scene_T*)scene_base))->actors, (actor_T*)focusable);

    return focusable;
}
