#include "include/component_pane.h"


component_pane_T* init_component_pane()
{
    component_pane_T* component_pane = calloc(1, sizeof(struct COMPONENT_PANE_STRUCT));
    component_pane->components = init_dynamic_list(sizeof(struct ACTOR_COMPONENT_STRUCT*));
    component_pane->component_index = 0;

    return component_pane;
}
