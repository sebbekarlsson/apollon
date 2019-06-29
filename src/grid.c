#include "include/grid.h"
#include <coelum/theatre.h>
#include <coelum/scene_manager.h>
#include <coelum/draw_utils.h>
#include <stdlib.h>
#include <png.h>
#include "include/image_utils.h"


extern theatre_T* THEATRE;

grid_cell_T* init_grid_cell()
{
    grid_cell_T* cell = calloc(1, sizeof(struct GRID_CELL_STRUCT));
    cell->r = 255;
    cell->g = 255;
    cell->b = 255;

    return cell;
}

grid_T* init_grid(
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
    grid_T* grid = calloc(1, sizeof(struct GRID_STRUCT));
    actor_T* actor = (actor_T*) grid;

    actor_constructor(actor, x, y, z, grid_tick, grid_draw, type_name);

    grid->width = width;
    grid->height = height;
    grid->cell_size = cell_size;
    grid->r = r;
    grid->g = g;
    grid->b = b;
    grid->cursor_x = width / 2;
    grid->cursor_y = height / 2;

    grid->cells = calloc(grid->width, sizeof(struct GRID_CELL_STRUCT***));
    for (int x = 0; x < grid->width; x++)
    {
        grid->cells[x] = calloc(grid->height, sizeof(struct GRID_CELL_STRUCT**));

        for (int y = 0; y < grid->height; y++)
        {
            grid->cells[x][y] = init_grid_cell();
        }
    }

    grid->focused = 0;

    return grid;
}

void grid_tick(actor_T* self)
{
    grid_T* grid = (grid_T*) self;

    if (grid->cursor_x > grid->width - 1)
    {
        grid->cursor_x = 0;
    }
    else
    if (grid->cursor_x < 0)
    {
        grid->cursor_x = grid->width - 1;
    }

    if (grid->cursor_y > grid->height - 1)
    {
        grid->cursor_y = 0;
    }
    else
    if (grid->cursor_y < 0)
    {
        grid->cursor_y = grid->height - 1;
    }
}

void grid_draw(actor_T* self)
{
    grid_T* grid = (grid_T*) self;

    scene_T* scene = scene_manager_get_current_scene(THEATRE->scene_manager);
    state_T* state = (state_T*) scene;

    // fill each square
    for (int x = 0; x < grid->width; x++)
    {
        float cell_x = self->x + (x * grid->cell_size);

        for (int y = 0; y < grid->height; y++)
        {
            float cell_y = self->y + (y * grid->cell_size);

            draw_positioned_2D_mesh(
                cell_x,
                cell_y,
                0.0f,
                grid->cell_size,
                grid->cell_size,
                grid->cells[x][y]->r,
                grid->cells[x][y]->g,
                grid->cells[x][y]->b,
                state
            );
        }
    }

    for (int x = 0; x < grid->width + 1; x++)
    {
        float cell_x = self->x + (x * grid->cell_size);

        // draw line downwards
        draw_line(
            cell_x,
            self->y,
            0.0f,
            cell_x,
            self->y + (grid->height * grid->cell_size),
            0.0f,
            grid->r + x == 0 || x == grid->width ? grid->focused * 255 : 0,
            grid->g,
            grid->b,
            state
        ); 

        for (int y = 0; y < grid->height + 1; y++)
        {

            float cell_y = self->y + (y * grid->cell_size);

            if (cell_x == self->x)
            {
                // draw lines sideways
                draw_line(
                    self->x,
                    cell_y,
                    0.0f,
                    self->x + (grid->width * grid->cell_size),
                    cell_y,
                    0.0f,
                    grid->r + y == 0 || y == grid->height ? grid->focused * 255 : 0,
                    grid->g,
                    grid->b,
                    state
                );
            } 

            if (grid->cursor_x == x && grid->cursor_y == y && grid->focused)
            {
                draw_positioned_2D_mesh(
                    cell_x,
                    cell_y,
                    0.0f,
                    grid->cell_size,
                    grid->cell_size,
                    0,
                    0,
                    0,
                    state
                );
            }
        }
    } 
}

int grid_create_image(grid_T* grid, const char* filename)
{
    bitmap_t grid_img;
    int x;
    int y;
    int status;

    status = 0;

    grid_img.width = grid->width;
    grid_img.height = grid->height;

    grid_img.pixels = calloc(grid_img.width * grid_img.height, sizeof(pixel_t));

    if (!grid_img.pixels)
    {
        return -1;
    }

    for (y = 0; y < grid_img.height; y++)
    {
        for (x = 0; x < grid_img.width; x++)
        {
            pixel_t * pixel = pixel_at (& grid_img, x, y);
            pixel->red = grid->cells[x][y]->r;
            pixel->green = grid->cells[x][y]->g;
            pixel->blue = grid->cells[x][y]->b;
        }
    }

    if (save_png_to_file(&grid_img, filename))
    {
        fprintf(stderr, "Error writing file.\n");
        status = -1;
    }

    free(grid_img.pixels);

    return status;
}
