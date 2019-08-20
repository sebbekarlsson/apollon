#include "include/focus_manager.h"
#include <coelum/input.h>


extern keyboard_state_T* KEYBOARD_STATE;


focus_manager_T* init_focus_manager()
{
    focus_manager_T* focus_manager = calloc(1, sizeof(struct FOCUS_MANAGER_STRUCT));

    focus_manager->components = init_dynamic_list(sizeof(struct ACTOR_FOCUSABLE_STRUCT*));
    focus_manager->components = init_dynamic_list(sizeof(struct ACTOR_COMPONENT_STRUCT*));
    focus_manager->focus_index = -1;

    return focus_manager;
}

void focus_manager_tick(focus_manager_T* focus_manager)
{ 
    if (focus_manager->components->size == 0)
        return;
}

void focus_manager_keep_disabled(focus_manager_T* focus_manager)
{
    focus_manager->focus_index = -1;

    for (int i = 0; i < focus_manager->components->size; i++)
        ((actor_component_T*)focus_manager->components->items[i])->focused = 0;

    return;
}

void focus_manager_unfocus(focus_manager_T* focus_manager)
{
    for (int i = 0; i < focus_manager->components->size; i++)
        ((actor_component_T*)focus_manager->components->items[i])->focused = 0;
}

void focus_manager_set_focused(focus_manager_T* focus_manager, actor_component_T* actor_component)
{
    focus_manager_unfocus(focus_manager);
    actor_component->focused = 1;
}

void focus_manager_free(focus_manager_T* focus_manager)
{
    free(focus_manager->components->items);
    free(focus_manager->components);
    free(focus_manager);
}
