#ifndef APOLLON_FOCUS_MANAGER_H
#define APOLLON_FOCUS_MANAGER_H
#include <coelum/dynamic_list.h>


typedef struct FOCUS_MANAGER_STRUCT
{
    dynamic_list_T* focusables;
    int focus_index;
} focus_manager_T;

focus_manager_T* init_focus_manager();

void focus_manager_tick(focus_manager_T* focus_manager);

void focus_manager_keep_disabled(focus_manager_T* focus_manager);

void focus_manager_free(focus_manager_T* focus_manager);
#endif
