#include "include/etc.h"
#include <coelum/input.h>
#include <coelum/theatre.h>


extern keyboard_state_T* KEYBOARD_STATE;
extern theatre_T* THEATRE;


void go_back_on_escape()
{
    if (KEYBOARD_STATE->keys[GLFW_KEY_ESCAPE] && !KEYBOARD_STATE->key_locks[GLFW_KEY_ESCAPE])
    {
        scene_manager_go_back(THEATRE->scene_manager);
        KEYBOARD_STATE->key_locks[GLFW_KEY_ESCAPE] = 1;
    }
}

void go_to_menu_on_escape()
{
    if (KEYBOARD_STATE->keys[GLFW_KEY_ESCAPE] && !KEYBOARD_STATE->key_locks[GLFW_KEY_ESCAPE])
    {
        scene_manager_goto(THEATRE->scene_manager, "menu");
        KEYBOARD_STATE->key_locks[GLFW_KEY_ESCAPE] = 1;
    }
}
