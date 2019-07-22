#include "include/focus_manager.h"
#include "include/actor_focusable.h"
#include <coelum/input.h>


extern keyboard_state_T* KEYBOARD_STATE;


focus_manager_T* init_focus_manager()
{
    focus_manager_T* focus_manager = calloc(1, sizeof(struct FOCUS_MANAGER_STRUCT));

    focus_manager->focusables = init_dynamic_list(sizeof(struct ACTOR_FOCUSABLE_STRUCT*));
    focus_manager->focus_index = -1;

    return focus_manager;
}

void focus_manager_tick(focus_manager_T* focus_manager)
{
    if (focus_manager->focusables->size == 0)
        return;

    if (KEYBOARD_STATE->keys[GLFW_KEY_TAB] && !KEYBOARD_STATE->key_locks[GLFW_KEY_TAB])
    {

        if (focus_manager->focus_index < focus_manager->focusables->size - 1)
            focus_manager->focus_index += 1;
        else
            focus_manager->focus_index = 0;
        
        while(((actor_focusable_T*)focus_manager->focusables->items[focus_manager->focus_index])->visible == 0)
        {
            if (focus_manager->focus_index < focus_manager->focusables->size - 1)
                focus_manager->focus_index += 1;
            else
                focus_manager->focus_index = 0;
        }

        KEYBOARD_STATE->key_locks[GLFW_KEY_TAB] = 1;
    }

    for (int i = 0; i < focus_manager->focusables->size; i++)
        ((actor_focusable_T*)focus_manager->focusables->items[i])->focused = 0;


    if (focus_manager->focus_index != -1)
    {
        actor_focusable_T* actor_focusable = (actor_focusable_T*) focus_manager->focusables->items[focus_manager->focus_index];
        actor_focusable->focused = 1;
    }
}
