#ifndef APOLLON_COMPONENT_PANE_H
#define APOLLON_COMPONENT_PANE_H
#include <coelum/dynamic_list.h>
#include <coelum/state.h>
#include <coelum/scene.h>
#include <glad/glad.h>
#include "actor_component.h"
#include "focus_manager.h"


typedef struct COMPONENT_PANE_STRUCT
{
    scene_T base;
    dynamic_list_T* components;
    dynamic_list_T* rows;
    dynamic_list_T* cols;
    focus_manager_T* focus_manager;
    int component_index;
    int width;
    int height;
    int x;
    int y;
    unsigned int centered;
    state_T* state;
} component_pane_T;

component_pane_T* init_component_pane(state_T* state, focus_manager_T* focus_manager, int x, int y, int width, int height);

void component_pane_tick(scene_T* self);

void component_pane_draw(scene_T* self);

actor_component_T* component_pane_add_component(component_pane_T* component_pane, actor_component_T* actor_component);

void component_pane_adjust(component_pane_T* component_pane);
#endif
