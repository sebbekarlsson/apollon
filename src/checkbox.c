#include "include/checkbox.h"
#include <coelum/current.h>
#include <coelum/draw_utils.h>
#include <coelum/input.h>


extern keyboard_state_T* KEYBOARD_STATE;

checkbox_T* init_checkbox(
    float x,
    float y,
    float z    
)
{
    checkbox_T* checkbox = calloc(1, sizeof(struct CHECKBOX_STRUCT));

    actor_focusable_T* actor_focusable = (actor_focusable_T*) checkbox;
    actor_focusable_constructor(actor_focusable);

    actor_T* actor = (actor_T*) actor_focusable;
    actor_constructor(actor, x, y, z, checkbox_tick, checkbox_draw, "checkbox");

    checkbox->width = 33;
    checkbox->height = 33;
    checkbox->checked = 0;

    return checkbox;
}

void checkbox_tick(actor_T* self)
{
    checkbox_T* checkbox = (checkbox_T*) self;
    actor_focusable_T* actor_focusable = (actor_focusable_T*) self;
    unsigned int focused = actor_focusable->focused;

    if (focused)
    {
        if (KEYBOARD_STATE->keys[GLFW_KEY_ENTER] && !KEYBOARD_STATE->key_locks[GLFW_KEY_ENTER])
        {
            if (checkbox->checked)
            {
                checkbox->checked = 0;
            }
            else
            {
                checkbox->checked = 1;
            }
            
            printf("checked: %d\n", checkbox->checked);

            KEYBOARD_STATE->key_locks[GLFW_KEY_ENTER] = 1;
        }
    }
}

void checkbox_draw(actor_T* self)
{
    scene_T* scene = get_current_scene();
    state_T* state = (state_T*) scene;

    checkbox_T* checkbox = (checkbox_T*) self;

    // top line
    draw_line(
        self->x,
        self->y,
        self->z,
        self->x + checkbox->width,
        self->y,
        self->z,
        0,
        0,
        0,
        state
    );

    // bottom line
    draw_line(
        self->x,
        self->y + checkbox->height,
        self->z,
        self->x + checkbox->width,
        self->y + checkbox->height,
        self->z,
        0,
        0,
        0,
        state
    );

    // left line
    draw_line(
        self->x,
        self->y,
        self->z,
        self->x,
        self->y + checkbox->height,
        self->z,
        0,
        0,
        0,
        state
    );

    // right line
    draw_line(
        self->x + checkbox->width,
        self->y,
        self->z,
        self->x + checkbox->width,
        self->y + checkbox->height,
        self->z,
        0,
        0,
        0,
        state
    );

    if (checkbox->checked)
    {
        unsigned int mesh_width = checkbox->width - 11;
        unsigned int mesh_height = checkbox->height - 11;

        draw_positioned_2D_mesh(
            self->x + (checkbox->width / 2) - (mesh_width / 2),
            self->y + (checkbox->height / 2) - (mesh_height / 2),
            self->z,
            mesh_width,
            mesh_height,
            0,
            0,
            0,
            state
        );
    }
}
