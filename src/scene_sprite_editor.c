#include "include/scene_sprite_editor.h"
#include "include/etc.h"
#include "include/database.h"
#include <coelum/constants.h>
#include <coelum/actor_text.h>
#include <coelum/input.h>
#include <coelum/utils.h>
#include <coelum/draw_utils.h>
#include <coelum/current.h>
#include <string.h>


extern keyboard_state_T* KEYBOARD_STATE;
extern database_T* DATABASE;


void sprite_button_save_press()
{
    scene_T* scene = get_current_scene();
    scene_sprite_editor_T* s_sprite_editor = (scene_sprite_editor_T*) scene;

    if (s_sprite_editor->sprite_index == -1)
    {
        printf("Create new sprite\n");

        sprite_T* sprite = init_sprite(
            scene_sprite_editor_get_frames_as_textures(s_sprite_editor),
            1,
            16,
            16
        );

        database_sprite_T* database_sprite = init_database_sprite(sprite, s_sprite_editor->input_field_name->value);

        dynamic_list_append(DATABASE->sprites, database_sprite);
        s_sprite_editor->sprite_index = DATABASE->sprites->size - 1;
    }
    else
    {
        printf("Modify sprite\n");
    }
}

void dropdown_list_sprite_press(void* option)
{
    printf("Selected a sprite!\n");
    dropdown_list_option_T* dropdown_list_option = (dropdown_list_option_T*) option;
    sprite_T* sprite = (sprite_T*) dropdown_list_option->value;
    
    scene_T* scene = get_current_scene();
    scene_sprite_editor_T* s_sprite_editor = (scene_sprite_editor_T*) scene;

    for (int i = 0; i < sprite->textures->size; i++)
    {
        texture_T* texture = (texture_T*) sprite->textures->items[i];

        grid_T* grid = init_grid(
            (WINDOW_WIDTH / 2) - ((16 * 16) / 2),
            (WINDOW_HEIGHT / 2) - ((16 * 16) / 2),
            0.0f,
            16,
            16,
            16,
            0,
            0,
            0,
            "grid_canvas"
        );

        int y, x;

        printf("%12.6f ? %d    %12.6f ? %d\n", grid->width, texture->width, grid->height, texture->height);
        for (y = 0; y < grid->height; y++)
        {
            for (x = 0; x < grid->width; x++)
            { 
                unsigned int channelCount = 4;
                unsigned bytePerPixel = channelCount;
                unsigned char* pixelOffset = texture->data + (y * (int)grid->width + x) * channelCount;
                unsigned char r = pixelOffset[0];
                unsigned char g = pixelOffset[1];
                unsigned char b = pixelOffset[2];
                //unsigned char a = channelCount >= 4 ? pixelOffset[3] : 0xff;

                grid->cells[x][y]->r = r;
                grid->cells[x][y]->g = g;
                grid->cells[x][y]->b = b;
                //grid->cells[y][x]->a = img[x][y][3] = (GLubyte) 255;

                printf("%d %d %d\n", r, g, b);
            }
        }
        
        dynamic_list_append(
            s_sprite_editor->grids,
            grid 
        );
    }

    scene_sprite_editor_refresh_grid(s_sprite_editor);
}

void sprite_button_new_press()
{
    printf("New!\n");
    scene_T* scene = get_current_scene();
    scene_sprite_editor_T* s_sprite_editor = (scene_sprite_editor_T*) scene;

    s_sprite_editor->sprite_index = -1;

    scene_sprite_editor_clear_all_frames(s_sprite_editor);
    
    memset(
        s_sprite_editor->input_field_name->value,
        0,
        sizeof(char) * strlen(s_sprite_editor->input_field_name->value)
    );
}

scene_sprite_editor_T* init_scene_sprite_editor()
{
    scene_sprite_editor_T* s_sprite_editor = calloc(1, sizeof(struct SCENE_SPRITE_EDITOR_STRUCT));
    scene_T* s = (scene_T*) s_sprite_editor;
    state_T* state = (state_T*) s; 

    scene_constructor(s, scene_sprite_editor_tick, scene_sprite_editor_draw, 2);

    s->type_name = "sprite_editor";
    s->bg_r = 255;
    s->bg_g = 255;
    s->bg_b = 255;

    s_sprite_editor->grid = init_grid(
        (WINDOW_WIDTH / 2) - ((16 * 16) / 2),
        (WINDOW_HEIGHT / 2) - ((16 * 16) / 2),
        0.0f,
        16,
        16,
        16,
        0,
        0,
        0,
        "grid_canvas"
    );

    // this one is starts as focused
    ((actor_focusable_T*)s_sprite_editor->grid)->focused = 1;

    s_sprite_editor->grids = init_dynamic_list(sizeof(struct GRID_STRUCT*));

    s_sprite_editor->grid_color_selector = init_grid(
        ((WINDOW_WIDTH / 2) - ((16 * 16) / 2)) + ((16 * 16) + 16),
        ((WINDOW_HEIGHT / 2) - ((16 * 16) / 2)),
        0.0f,
        4,
        8,
        16,
        0,
        0,
        0,
        "grid_color_selector"
    );

    // state representation of first grid in frame list.
    dynamic_list_append(
        s_sprite_editor->grids,
        init_grid(
            (WINDOW_WIDTH / 2) - ((16 * 16) / 2),
            (WINDOW_HEIGHT / 2) - ((16 * 16) / 2),
            0.0f,
            16,
            16,
            16,
            0,
            0,
            0,
            "grid_canvas"
        )
    );

    float r, g, b = 255.0f;

    for (int x = 0; x < s_sprite_editor->grid_color_selector->width; x++)
    {
        for (int y = 0; y < s_sprite_editor->grid_color_selector->height; y++)
        {
            if (x == 0)
            {
                r = 255 - (y * (255 / 8));
                g = 0;
                b = 0;
            }
            else
            if (x == 1)
            {
                r = 0;
                g = 255 - (y * (255 / 8));
                b = 0;
            }
            else
            if (x == 2)
            {
                r = 0;
                g = 0;
                b = 255 - (y * (255 / 8));
            }
            else
            if (x == 3)
            {
                r = 255 - (y * (255 / 8));
                g = 255 - (y * (255 / 8));
                b = 255 - (y * (255 / 8));
            }

            s_sprite_editor->grid_color_selector->cells[x][y]->r = r;
            s_sprite_editor->grid_color_selector->cells[x][y]->g = g;
            s_sprite_editor->grid_color_selector->cells[x][y]->b = b;
        }
    }

    s_sprite_editor->grid_color_mixer = init_grid(
        ((WINDOW_WIDTH / 2) - ((16 * 16) / 2)) + ((16 * 16) + 16),
        ((WINDOW_HEIGHT / 2) - ((16 * 16) / 2)) + ((16 * 8) + 16),
        0.0f,
        8,
        7,
        16,
        0,
        0,
        0,
        "grid_color_mixer"
    );
    
    dynamic_list_append(state->actors, s_sprite_editor->grid);
    dynamic_list_append(state->actors, s_sprite_editor->grid_color_selector);
    dynamic_list_append(state->actors, s_sprite_editor->grid_color_mixer);

    s_sprite_editor->focus_manager = init_focus_manager();

    s_sprite_editor->label_name = init_label(
        (WINDOW_WIDTH / 2) - ((16 * 16) / 2),
        (WINDOW_HEIGHT / 2) - ((16 * 16) / 2) - 80,
        0.0f,
        "Name"
    );
    dynamic_list_append(state->actors, s_sprite_editor->label_name);

    s_sprite_editor->input_field_name = init_input_field(
        (WINDOW_WIDTH / 2) - ((16 * 16) / 2),
        (WINDOW_HEIGHT / 2) - ((16 * 16) / 2) - 64,
        0.0f
    );
    dynamic_list_append(state->actors, s_sprite_editor->input_field_name);
    dynamic_list_append(s_sprite_editor->focus_manager->focusables, s_sprite_editor->input_field_name);

    s_sprite_editor->button_save = init_button(
        (WINDOW_WIDTH / 2) - ((16 * 16) / 2),
        (WINDOW_HEIGHT / 2) + ((16 * 16) / 2) + 16,
        0.0f,
        "Save",
        sprite_button_save_press
    );
    dynamic_list_append(state->actors, s_sprite_editor->button_save);
    dynamic_list_append(s_sprite_editor->focus_manager->focusables, s_sprite_editor->button_save);

    s_sprite_editor->button_new = init_button(
        (WINDOW_WIDTH / 2) - ((16 * 16) / 2) + 216,
        (WINDOW_HEIGHT / 2) + ((16 * 16) / 2) + 16,
        0.0f,
        "New",
        sprite_button_new_press
    );
    dynamic_list_append(state->actors, s_sprite_editor->button_new);
    dynamic_list_append(s_sprite_editor->focus_manager->focusables, s_sprite_editor->button_new);
    
    int dropdown_list_sprite_width = 160;
    s_sprite_editor->label_name = init_label(
        ((WINDOW_WIDTH / 2) - ((16 * 16) / 2)) - (dropdown_list_sprite_width + 16),
        (WINDOW_HEIGHT / 2) - ((16 * 16) / 2) - (8 + 8),
        0.0f,
        "Sprite"
    );
    dynamic_list_append(state->actors, s_sprite_editor->label_name);

    s_sprite_editor->dropdown_list_sprite = init_dropdown_list(
        (((WINDOW_WIDTH / 2) - ((16 * 16) / 2))) - (dropdown_list_sprite_width + 16),
        (WINDOW_HEIGHT / 2) - ((16 * 16) / 2),
        0.0f,
        dropdown_list_sprite_press
    );
    s_sprite_editor->dropdown_list_sprite->expanded = 0;
    s_sprite_editor->dropdown_list_sprite->width = dropdown_list_sprite_width;
    ((actor_T*)s_sprite_editor->dropdown_list_sprite)->z = 1;

    dynamic_list_append(s_sprite_editor->focus_manager->focusables, (actor_focusable_T*) s_sprite_editor->dropdown_list_sprite);
    dynamic_list_append(state->actors, s_sprite_editor->dropdown_list_sprite);


    dynamic_list_append(s_sprite_editor->focus_manager->focusables, s_sprite_editor->grid);
    dynamic_list_append(s_sprite_editor->focus_manager->focusables, s_sprite_editor->grid_color_selector);
    dynamic_list_append(s_sprite_editor->focus_manager->focusables, s_sprite_editor->grid_color_mixer);

    s_sprite_editor->grid_index = 0;
    s_sprite_editor->sprite_index = -1;

    s_sprite_editor->r = 0.0f;
    s_sprite_editor->g = 0.0f;
    s_sprite_editor->b = 0.0f;

    return s_sprite_editor;
}

void scene_sprite_editor_tick(scene_T* self)
{
    go_back_on_escape();

    scene_sprite_editor_T* s_sprite_editor = (scene_sprite_editor_T*) self;
    
    focus_manager_tick(s_sprite_editor->focus_manager);

    actor_focusable_T* actor_focusable = (actor_focusable_T*) s_sprite_editor->focus_manager->focusables->items[s_sprite_editor->focus_manager->focus_index];
    actor_T* grid_actor = (actor_T*) actor_focusable;
    grid_T* grid = (grid_T*) grid_actor;
    actor_focusable->focused = 1;

    if (KEYBOARD_STATE->keys[GLFW_KEY_UP] && !KEYBOARD_STATE->key_locks[GLFW_KEY_UP])
    {
        grid->cursor_y -= 1;
        KEYBOARD_STATE->key_locks[GLFW_KEY_UP] = 1;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_DOWN] && !KEYBOARD_STATE->key_locks[GLFW_KEY_DOWN])
    {
        grid->cursor_y += 1;
        KEYBOARD_STATE->key_locks[GLFW_KEY_DOWN] = 1;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_LEFT] && !KEYBOARD_STATE->key_locks[GLFW_KEY_LEFT])
    {
        grid->cursor_x -= 1;
        KEYBOARD_STATE->key_locks[GLFW_KEY_LEFT] = 1;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_RIGHT] && !KEYBOARD_STATE->key_locks[GLFW_KEY_RIGHT])
    {
        grid->cursor_x += 1;
        KEYBOARD_STATE->key_locks[GLFW_KEY_RIGHT] = 1;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_Z] && !KEYBOARD_STATE->key_locks[GLFW_KEY_Z])
    {
        scene_sprite_editor_goto_prev(s_sprite_editor);
        KEYBOARD_STATE->key_locks[GLFW_KEY_Z] = 1;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_X] && !KEYBOARD_STATE->key_locks[GLFW_KEY_X])
    {
        scene_sprite_editor_goto_next(s_sprite_editor);
        KEYBOARD_STATE->key_locks[GLFW_KEY_X] = 1;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_C] && !KEYBOARD_STATE->key_locks[GLFW_KEY_C])
    {
        scene_sprite_editor_delete_current_frame(s_sprite_editor);
        KEYBOARD_STATE->key_locks[GLFW_KEY_C] = 1;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_S] && !KEYBOARD_STATE->key_locks[GLFW_KEY_S])
    {
        grid_create_image(s_sprite_editor->grid, "sheet.png");
        KEYBOARD_STATE->key_locks[GLFW_KEY_S] = 1;
    }

    if (strcmp(grid_actor->type_name, "grid_canvas") == 0)
    {
        if (KEYBOARD_STATE->keys[GLFW_KEY_SPACE])
        {
            if (
               grid->cursor_x < 0 ||
               grid->cursor_x > grid->width - 1 ||
               grid->cursor_y < 0 ||
               grid->cursor_y > grid->height - 1
            )
            {
                return;
            }

            grid->cells[grid->cursor_x][grid->cursor_y]->r = s_sprite_editor->r;
            grid->cells[grid->cursor_x][grid->cursor_y]->g = s_sprite_editor->g;
            grid->cells[grid->cursor_x][grid->cursor_y]->b = s_sprite_editor->b;
        }

        // save current state of the grid into the representation in our state list.
        grid_copy(
            s_sprite_editor->grid,
            s_sprite_editor->grids->items[s_sprite_editor->grid_index]
        );
    }
    else
    if (strcmp(grid_actor->type_name, "grid_color_selector") == 0 || strcmp(grid_actor->type_name, "grid_color_mixer") == 0)
    {
        if (KEYBOARD_STATE->keys[GLFW_KEY_G])
        {
            if (
               grid->cursor_x < 0 ||
               grid->cursor_x > grid->width - 1 ||
               grid->cursor_y < 0 ||
               grid->cursor_y > grid->height - 1
            )
            {
                return;
            }

            s_sprite_editor->r = grid->cells[grid->cursor_x][grid->cursor_y]->r;
            s_sprite_editor->g = grid->cells[grid->cursor_x][grid->cursor_y]->g;
            s_sprite_editor->b = grid->cells[grid->cursor_x][grid->cursor_y]->b;
        }
    }

    if (strcmp(grid_actor->type_name, "grid_color_mixer") == 0)
    {
        if (KEYBOARD_STATE->keys[GLFW_KEY_SPACE] && ! KEYBOARD_STATE->key_locks[GLFW_KEY_SPACE])
        {
            if (
               grid->cursor_x < 0 ||
               grid->cursor_x > grid->width - 1 ||
               grid->cursor_y < 0 ||
               grid->cursor_y > grid->height - 1
            )
            {
                return;
            }

            if (
                grid->cells[grid->cursor_x][grid->cursor_y]->r == 255 &&
                grid->cells[grid->cursor_x][grid->cursor_y]->g == 255 &&
                grid->cells[grid->cursor_x][grid->cursor_y]->b == 255
            )
            {
                grid->cells[grid->cursor_x][grid->cursor_y]->r = 0;
                grid->cells[grid->cursor_x][grid->cursor_y]->g = 0;
                grid->cells[grid->cursor_x][grid->cursor_y]->b = 0;
            }

            grid->cells[grid->cursor_x][grid->cursor_y]->r = (s_sprite_editor->r + grid->cells[grid->cursor_x][grid->cursor_y]->r);
            grid->cells[grid->cursor_x][grid->cursor_y]->g = (s_sprite_editor->g + grid->cells[grid->cursor_x][grid->cursor_y]->g);
            grid->cells[grid->cursor_x][grid->cursor_y]->b = (s_sprite_editor->b + grid->cells[grid->cursor_x][grid->cursor_y]->b);

            KEYBOARD_STATE->key_locks[GLFW_KEY_SPACE] = 1;
        }
    }

    if (s_sprite_editor->dropdown_list_sprite->options->size < DATABASE->sprites->size)
    {
        for (int i = s_sprite_editor->dropdown_list_sprite->options->size; i < DATABASE->sprites->size; i++)
        {
            database_sprite_T* database_sprite = DATABASE->sprites->items[i];

            char* name = calloc(strlen(database_sprite->name) + 1, sizeof(char));
            strcpy(name, database_sprite->name);

            // 20 = (sprite_width(16) + margin(4))
            // 12 = (font_size + font_spacing)
            unsigned int text_limit = ((s_sprite_editor->dropdown_list_sprite->width - 20) / (12)) - 1;

            dynamic_list_append(
                s_sprite_editor->dropdown_list_sprite->options,
                init_dropdown_list_option(
                    database_sprite->sprite,
                    name,
                    database_sprite->sprite,
                    text_limit
                )
            );
        }
    }
}

void scene_sprite_editor_draw(scene_T* self)
{
    state_T* state = (state_T*) self;
    scene_sprite_editor_T* s_sprite_editor = (scene_sprite_editor_T*) self;

    char grid_index_str[16];
    sprintf(grid_index_str, "%d / %d", (int)s_sprite_editor->grid_index, (int)s_sprite_editor->grids->size - 1);

    draw_text(
        grid_index_str,
        ((actor_T*)s_sprite_editor->grid)->x + (16 / 2),
        ((actor_T*)s_sprite_editor->grid)->y - (16 / 2),
        0,
        0, // r
        0, // g
        0, // b
        6,
        6,
        0,
        state
    );
}

void scene_sprite_editor_goto_next(scene_sprite_editor_T* self)
{
    self->grid_index += 1;

    if (self->grids->size-1 < self->grid_index)
    {
        printf("Created another grid state representation!\n");

        dynamic_list_append(
            self->grids,
            init_grid(
                (WINDOW_WIDTH / 2) - ((16 * 16) / 2),
                (WINDOW_HEIGHT / 2) - ((16 * 16) / 2),
                0.0f,
                16,
                16,
                16,
                0,
                0,
                0,
                "grid_canvas"
            )
        );
    }

    scene_sprite_editor_refresh_grid(self);
}

void scene_sprite_editor_goto_prev(scene_sprite_editor_T* self)
{
    if (self->grid_index > 0)
        self->grid_index -= 1;

    scene_sprite_editor_refresh_grid(self);
}

void _grid_free(void* item)
{
    grid_free((grid_T*) item);
}

void scene_sprite_editor_delete_current_frame(scene_sprite_editor_T* self)
{
    if (self->grid_index == 0)
    {
        printf("Cannot delete first frame\n");
        return;
    }
    
    grid_T* current_grid_state = (grid_T*) self->grids->items[self->grid_index];

    dynamic_list_remove(self->grids, current_grid_state, _grid_free);

    self->grid_index -= 1;

    scene_sprite_editor_refresh_grid(self);
}

void scene_sprite_editor_clear_all_frames(scene_sprite_editor_T* self)
{
    for (int i = self->grids->size; i > 0; i--)
    {
        grid_T* grid_state = (grid_T*) self->grids->items[i];

        dynamic_list_remove(self->grids, grid_state, _grid_free);

        self->grid_index -= 1;
    }

    self->grid_index = 0;

    scene_sprite_editor_refresh_grid(self);
}

void scene_sprite_editor_refresh_grid(scene_sprite_editor_T* self)
{
    if (self->grids->size == 0)
    {
        printf("Cannot refresh grid!\n");
        grid_clean(self->grid);
        return;
    }
    
    grid_T* current_grid_state = (grid_T*) self->grids->items[self->grid_index]; 
    
    grid_clean(self->grid);
    //grid_clean(current_grid_state);

    grid_copy(current_grid_state, self->grid);
}

dynamic_list_T* scene_sprite_editor_get_frames_as_textures(scene_sprite_editor_T* self)
{
    dynamic_list_T* frame_list = init_dynamic_list(sizeof(struct TEXTURE_STRUCT*));

    for (int i = 0; i < self->grids->size; i++)
    {
        grid_T* grid = (grid_T*) self->grids->items[i];

        dynamic_list_append(frame_list, grid_create_texture(grid));
    }

    return frame_list;
}
