#include "include/component_grid.h"
#include <coelum/current.h>
#include <coelum/scene_manager.h>
#include <coelum/draw_utils.h>
#include <stdlib.h>
#include <png.h>
#include <glad/glad.h>
#include "include/image_utils.h"


extern const float COLOR_FG[3];

extern sprite_T* SPRITE_CHECKBOARD;


static void grid_on_click(actor_T* self)
{
}

component_grid_cell_T* init_component_grid_cell()
{
    component_grid_cell_T* cell = calloc(1, sizeof(struct COMPONENT_GRID_CELL_STRUCT));
    cell->r = 255;
    cell->g = 255;
    cell->b = 255;
    cell->a = 0.0f;
    cell->sprite = (void*) 0;
    cell->selected = -1;

    return cell;
}

component_grid_T* init_component_grid(
    focus_manager_T* focus_manager,
    float x,
    float y,
    float z,
    float width,
    float height,
    float cell_size,
    float r,
    float g,
    float b,
    char* type_name
)
{
    component_grid_T* component_grid = calloc(1, sizeof(struct COMPONENT_GRID_STRUCT));
    actor_T* actor = (actor_T*) component_grid;

    actor_constructor(actor, x, y, z, component_grid_tick, component_grid_draw, type_name);

    actor_component_T* actor_component = (actor_component_T*) component_grid;

    actor_component_constructor(actor_component, focus_manager, grid_on_click);

    component_grid->width = width;
    actor->width = component_grid->width * cell_size;
    component_grid->height = height;
    actor->height = component_grid->height * cell_size;
    component_grid->cell_size = cell_size;
    component_grid->r = r;
    component_grid->g = g;
    component_grid->b = b;
    component_grid->cursor_x = width / 2;
    component_grid->cursor_y = height / 2;

    component_grid->cells = calloc(component_grid->width, sizeof(struct COMPONENT_GRID_CELL_STRUCT***));
    for (int x = 0; x < component_grid->width; x++)
    {
        component_grid->cells[x] = calloc(component_grid->height, sizeof(struct COMPONENT_GRID_CELL_STRUCT**));

        for (int y = 0; y < component_grid->height; y++)
        {
            component_grid->cells[x][y] = init_component_grid_cell();
        }
    }

    component_grid->onion = (void*)0;

    return component_grid;
}

void component_grid_tick(actor_T* self)
{
    component_grid_T* component_grid = (component_grid_T*) self;
    actor_component_tick((actor_component_T*) component_grid);

    if (component_grid->cursor_x > component_grid->width - 1)
    {
        component_grid->cursor_x = 0;
    }
    else
    if (component_grid->cursor_x < 0)
    {
        component_grid->cursor_x = component_grid->width - 1;
    }

    if (component_grid->cursor_y > component_grid->height - 1)
    {
        component_grid->cursor_y = 0;
    }
    else
    if (component_grid->cursor_y < 0)
    {
        component_grid->cursor_y = component_grid->height - 1;
    }
}

void component_grid_draw(actor_T* self)
{
    component_grid_T* component_grid = (component_grid_T*) self;
    actor_component_T* actor_component = (actor_component_T*) component_grid;

    scene_T* scene = get_current_scene();
    state_T* state = (state_T*) scene;

    // fill each square
    for (int x = 0; x < component_grid->width; x++)
    {
        float cell_x = self->x + (x * component_grid->cell_size);

        for (int y = 0; y < component_grid->height; y++)
        {
            float cell_y = self->y + (y * component_grid->cell_size);

            component_grid_cell_T* cell = component_grid->cells[x][y];

            if (cell->sprite == (void*) 0)
            {
                draw_positioned_sprite(
                    SPRITE_CHECKBOARD,
                    cell_x,
                    cell_y,
                    self->z + 0.1f,
                    component_grid->cell_size,
                    component_grid->cell_size,
                    state
                ); 
            }
            else
            {
                draw_positioned_sprite(
                    cell->sprite,
                    cell_x,
                    cell_y,
                    self->z + 0.1f,
                    component_grid->cell_size,
                    component_grid->cell_size,
                    state
                );
            }

            // onion
            if (component_grid->onion != (void*)0)
            {
                component_grid_T* onion = (component_grid_T*) component_grid->onion;

                if (onion->cells[x][y]->a >= 1)
                    draw_positioned_2D_mesh(
                        cell_x,
                        cell_y,
                        self->z + 0.2f,
                        onion->cell_size,
                        onion->cell_size,
                        onion->cells[x][y]->r,
                        onion->cells[x][y]->g,
                        onion->cells[x][y]->b,
                        0.5f,
                        state
                    );
            }

            if (!(component_grid->cursor_x == x && component_grid->cursor_y == y && (actor_component->focused || actor_component->hovered) && cell->selected == 0))
            {
                draw_positioned_2D_mesh(
                    cell_x,
                    cell_y,
                    self->z + 0.25f,
                    component_grid->cell_size,
                    component_grid->cell_size,
                    cell->selected == 0 ? 0 : component_grid->cells[x][y]->r,
                    cell->selected == 0 ? 0 : component_grid->cells[x][y]->g,
                    cell->selected == 0 ? 0 : component_grid->cells[x][y]->b,
                    cell->selected == 0 ? 0.6 : component_grid->cells[x][y]->a,
                    state
                );
            }
        }
    }

    for (int x = 0; x < component_grid->width + 1; x++)
    {
        float cell_x = self->x + (x * component_grid->cell_size);

        // draw line downwards
        draw_line(
            cell_x,
            self->y,
            self->z + 0.4f,
            cell_x,
            self->y + (component_grid->height * component_grid->cell_size),
            self->z + 0.4f,
            component_grid->r + x == 0 || x == component_grid->width ? (actor_component->focused || actor_component->hovered) * 255 : 0,
            component_grid->g,
            component_grid->b,
            state
        ); 

        for (int y = 0; y < component_grid->height + 1; y++)
        {
            float cell_y = self->y + (y * component_grid->cell_size);

            float cell_r = 255;
            float cell_g = 255;
            float cell_b = 255;

            if (x < component_grid->width && y < component_grid->height)
            {
                cell_r = component_grid->cells[x][y]->r;
                cell_g = component_grid->cells[x][y]->g;
                cell_b = component_grid->cells[x][y]->b;
            }

            if (cell_x == self->x)
            {
                // draw lines sideways
                draw_line(
                    self->x,
                    cell_y,
                    self->z + 0.4f,
                    self->x + (component_grid->width * component_grid->cell_size),
                    cell_y,
                    self->z + 0.4f,
                    component_grid->r + y == 0 || y == component_grid->height ? (actor_component->focused || actor_component->hovered) * 255 : 0,
                    component_grid->g,
                    component_grid->b,
                    state
                );
            } 

            if (component_grid->cursor_x == x && component_grid->cursor_y == y && (actor_component->focused || actor_component->hovered))
            {
                if (x < component_grid->width && y < component_grid->height)
                {
                    component_grid_cell_T* cell = component_grid->cells[x][y];

                    if (cell->selected == -1)
                    {
                        draw_positioned_2D_mesh(
                            cell_x,
                            cell_y,
                            self->z + 0.3f,
                            component_grid->cell_size,
                            component_grid->cell_size,
                            255 - cell_r,
                            255 - cell_g,
                            255 - cell_b,
                            cell->sprite == (void*) 0 ? 1.0f : 0.6,
                            state
                        );
                    }
                }
            }
        }
    } 
}

int component_grid_create_image(component_grid_T* component_grid, const char* filename)
{
    bitmap_t component_grid_img;
    int x;
    int y;
    int status;

    status = 0;

    component_grid_img.width = component_grid->width;
    component_grid_img.height = component_grid->height;

    component_grid_img.pixels = calloc(component_grid_img.width * component_grid_img.height, sizeof(pixel_t));

    if (!component_grid_img.pixels)
        return -1;

    for (y = 0; y < component_grid_img.height; y++)
    {
        for (x = 0; x < component_grid_img.width; x++)
        {
            pixel_t * pixel = pixel_at (& component_grid_img, x, y);
            pixel->red = component_grid->cells[x][y]->r;
            pixel->green = component_grid->cells[x][y]->g;
            pixel->blue = component_grid->cells[x][y]->b;
        }
    }

    if (save_png_to_file(&component_grid_img, filename))
    {
        fprintf(stderr, "Error writing file.\n");
        status = -1;
    }

    free(component_grid_img.pixels);

    return status;
}

texture_T* component_grid_create_texture(component_grid_T* component_grid)
{
    unsigned char* img = calloc((component_grid->width * component_grid->height) * 4, sizeof(unsigned char));

    int x = 0;
    int y = 0;
    for (int i = 0; i < component_grid->width * component_grid->height; i+=1)
    {
        y = i / (int) component_grid->width;
        x = i % (int) component_grid->width;

        img[(4 * i)] = component_grid->cells[x][y]->r;
        img[(4 * i) + 1] = component_grid->cells[x][y]->g;
        img[(4 * i) + 2] = component_grid->cells[x][y]->b;
        img[(4 * i) + 3] = (int)(component_grid->cells[x][y]->a * 255);
    }

    return get_texture_from_data((unsigned char*)img, component_grid->width, component_grid->height, GL_RGBA);
}

void component_grid_copy(component_grid_T* source_component_grid, component_grid_T* target_component_grid)
{
    for (int x = 0; x < source_component_grid->width; x++)
    {
        for (int y = 0; y < source_component_grid->height; y++)
        {
            target_component_grid->cells[x][y]->r = source_component_grid->cells[x][y]->r;
            target_component_grid->cells[x][y]->g = source_component_grid->cells[x][y]->g;
            target_component_grid->cells[x][y]->b = source_component_grid->cells[x][y]->b;
            target_component_grid->cells[x][y]->a = source_component_grid->cells[x][y]->a;
        }
    }
}

void component_grid_unselect_all_cells(component_grid_T* component_grid)
{
    for (int x = 0; x < component_grid->width; x++)
    {
        for (int y = 0; y < component_grid->height; y++)
        {
            component_grid->cells[x][y]->selected = 0;
        }
    }
}

void component_grid_clean(component_grid_T* component_grid)
{
    for (int x = 0; x < component_grid->width; x++)
    {
        for (int y = 0; y < component_grid->height; y++)
        {
            component_grid->cells[x][y]->r = 255;
            component_grid->cells[x][y]->g = 255;
            component_grid->cells[x][y]->b = 255;
            component_grid->cells[x][y]->a = 0.0f;
        }
    }
}

void component_grid_free(component_grid_T* component_grid)
{
    for (int x = 0; x < component_grid->width; x++)
    {
        for (int y = 0; y < component_grid->height; y++)
        {
            free(component_grid->cells[x][y]);
        }

        free(component_grid->cells[x]);
    }

    free(component_grid);
}
