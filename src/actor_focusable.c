#include "include/actor_focusable.h"


actor_focusable_T* actor_focusable_constructor(actor_focusable_T* actor_focusable)
{
    actor_focusable->focused = 0;

    return actor_focusable;
}
