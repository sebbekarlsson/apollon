#ifndef APOLLON_SELECT_LIST_H
#define APOLLON_SELECT_LIST_H
#include <coelum/dynamic_list.h>
#include <coelum/state.h>


typedef struct SELECT_LIST_STRUCT
{
    dynamic_list_T* options;
    unsigned int option_index;
    state_T* state;
} select_list_T;

select_list_T* init_select_list(state_T* state);

void select_list_tick(select_list_T* select_list);

typedef struct SCENE_MENU_OPTION_STRUCT
{
    char* name;
    void (*callback)();
} select_list_option_T;

select_list_option_T* select_list_register_option(select_list_T* select_list, char* name, void (*callback)());
#endif
