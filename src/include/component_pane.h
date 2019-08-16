#ifndef APOLLON_COMPONENT_PANE_H
#define APOLLON_COMPONENT_PANE_H
#include <coelum/dynamic_list.h>


typedef struct COMPONENT_PANE_STRUCT
{
    dynamic_list_T* components;
    int component_index;
} component_pane_T;

component_pane_T* init_component_pane();
#endif
