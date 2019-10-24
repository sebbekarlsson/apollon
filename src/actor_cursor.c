#include "include/actor_cursor.h"
#include <coelum/draw_utils.h>
#include <coelum/current.h>


extern const float COLOR_FG[3];


actor_cursor_T* init_actor_cursor(float x, float y, float z)
{
    actor_cursor_T* actor_cursor = calloc(1, sizeof(struct ACTOR_CURSOR_STRUCT));
    actor_T* actor = (actor_T*) actor_cursor;
    actor_constructor(actor, x, y, z, actor_cursor_tick, actor_cursor_draw, "actor_cursor");

    actor_cursor->length = 16;

    return actor_cursor;
}

void actor_cursor_tick(actor_T* self) {}

void actor_cursor_draw(actor_T* self)
{
    actor_cursor_T* actor_cursor = (actor_cursor_T*) self;
    scene_T* scene = (scene_T*) get_current_scene();
    state_T* state = (state_T*) scene;

    // 8 == half a cell

    draw_line(
        (self->x + 8) - (actor_cursor->length / 2),
        (self->y + 8),
        self->z,
        (self->x + 8) + (actor_cursor->length / 2),
        (self->y + 8),
        self->z,
        COLOR_FG[0],
        COLOR_FG[1],
        COLOR_FG[2],
        state
    );

    draw_line(
        (self->x + 8),
        (self->y + 8) - (actor_cursor->length / 2),
        self->z,
        (self->x + 8),
        (self->y + 8) + (actor_cursor->length / 2),
        self->z,
        COLOR_FG[0],
        COLOR_FG[1],
        COLOR_FG[2],
        state
    );
}
