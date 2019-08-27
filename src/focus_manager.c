#include "include/focus_manager.h"
#include <coelum/input.h>
#include <GLFW/glfw3.h>


extern keyboard_state_T* KEYBOARD_STATE;
extern mouse_state_T* MOUSE_STATE;
extern GLFWwindow* window;


focus_manager_T* init_focus_manager()
{
    focus_manager_T* focus_manager = calloc(1, sizeof(struct FOCUS_MANAGER_STRUCT));

    focus_manager->components = init_dynamic_list(sizeof(struct ACTOR_COMPONENT_STRUCT*));
    focus_manager->focus_index = -1;

    return focus_manager;
}

static int compare(const void *s1, const void *s2)
{
    const actor_component_T **a = (const actor_component_T **)s1;
    const actor_component_T **b = (const actor_component_T **)s2;

    return (*b)->prio - (*a)->prio;
}

static void focus_manager_sort_components(focus_manager_T* focus_manager)
{
    qsort(focus_manager->components->items, focus_manager->components->size, sizeof(struct ACTOR_COMPONENT_STRUCT*), compare);
}

void focus_manager_tick(focus_manager_T* focus_manager)
{ 
    if (focus_manager->components->size == 0)
        return;

    focus_manager_sort_components(focus_manager);

    unsigned int click = 0;
    static int oldState = GLFW_RELEASE;
    int newState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (newState == GLFW_RELEASE && oldState == GLFW_PRESS) {
       click = 1;
    }
    oldState = newState;

    for (int i = 0; i < focus_manager->components->size; i++)
    {
        actor_component_T* actor_component = (actor_component_T*) focus_manager->components->items[i];
        actor_T* actor = (actor_T*) actor_component;

        actor_component->hovered = 0;

        if (MOUSE_STATE->x >= actor->x && MOUSE_STATE->x <= actor->x + actor->width)
        {
            if (MOUSE_STATE->y >= actor->y && MOUSE_STATE->y <= actor->y + actor->height)
            {
                actor_component->hovered = 1;

                printf("wtf\n");

                if (click)
                {
                    if (actor_component->on_click)
                    {
                        actor_component->on_click(actor);
                    }

                    focus_manager_set_focused(actor_component->focus_manager, actor_component);
                    
                    break;
                }

                break;
            }
        }
    }
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
