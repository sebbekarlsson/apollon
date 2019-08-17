#ifndef APOLLON_COMPONENT_PANE_H
#define APOLLON_COMPONENT_PANE_H
#include <coelum/dynamic_list.h>
#include <coelum/state.h>
#include <coelum/scene.h>
#include "actor_component.h"


typedef struct COMPONENT_PANE_STRUCT
{
    enum
    {
        COMPONENT_PANE_POSITION_ROW,
        COMPONENT_PANE_POSITION_COL
    } position;

    scene_T base;
    dynamic_list_T* components;
    int component_index;
    int width;
    int height;
    int x;
    int y;
    state_T* state;
} component_pane_T;

component_pane_T* init_component_pane(state_T* state, int x, int y, int width, int height);

void component_pane_tick(scene_T* self);

void component_pane_draw(scene_T* self);

actor_component_T* component_pane_add_component(component_pane_T* component_pane, actor_component_T* actor_component);

void component_pane_adjust(component_pane_T* component_pane);
#endif
