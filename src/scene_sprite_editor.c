#include "include/main.h"
#include "include/scene_sprite_editor.h"
#include "include/etc.h"
#include "include/modal.h"
#include "include/modal_manager.h"
#include <coelum/file_utils.h>
#include <coelum/constants.h>
#include <coelum/actor_text.h>
#include <coelum/input.h>
#include <coelum/utils.h>
#include <coelum/draw_utils.h>
#include <coelum/current.h>
#include <string.h>


extern keyboard_state_T* KEYBOARD_STATE;
extern database_T* DATABASE;
extern main_state_T* MAIN_STATE;
extern modal_manager_T* MODAL_MANAGER;
extern sprite_T* SPRITE_PENCIL;
extern sprite_T* SPRITE_ERASOR;

#define REFRESH_STATE(s_sprite_editor)\
    ((scene_base_T*)s_sprite_editor)->refresh_state((scene_base_T*)s_sprite_editor);


static void scene_sprite_editor_clear_input_fields(scene_sprite_editor_T* s_sprite_editor)
{
    memset(
        s_sprite_editor->input_field_name->value,
        '\0',
        strlen(s_sprite_editor->input_field_name->value) * sizeof(char)
    ); 

    memset(
        s_sprite_editor->input_field_frame_delay->value,
        '\0',
        strlen(s_sprite_editor->input_field_frame_delay->value) * sizeof(char)
    );
}

static void _free_sprite_dropdown_option(void* item)
{
    dropdown_list_option_T* dropdown_list_option = (dropdown_list_option_T*) item;
    
    free(dropdown_list_option->key);
    free((char*)dropdown_list_option->value);
    free(dropdown_list_option);
}

static void scene_sprite_editor_set_tool(scene_sprite_editor_T* s_sprite_editor, unsigned int tool_index)
{
    s_sprite_editor->tool_index = tool_index;
    grid_unselect_all_cells(s_sprite_editor->grid_tool_selector);
}

void scene_sprite_editor_refresh_state(scene_base_T* scene_base)
{
    scene_sprite_editor_T* s_sprite_editor = (scene_sprite_editor_T*) scene_base;

    dropdown_list_sync_from_table(
        s_sprite_editor->dropdown_list_sprite,
        DATABASE,
        "sprites",
        1,
        0
    );

    dropdown_list_reload_sprites(s_sprite_editor->dropdown_list_sprite);

    if (s_sprite_editor->sprite_id != (void*) 0)
    {
        dropdown_list_set_selected_option_by_string_value(
            s_sprite_editor->dropdown_list_sprite,
            (const char*) s_sprite_editor->sprite_id 
        );
    }
    else
    {
        scene_sprite_editor_clear_input_fields(s_sprite_editor);
        scene_sprite_editor_clear_all_frames(s_sprite_editor);

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
    }
}

void scene_sprite_editor_delete_grids_reset_sprite_id(scene_sprite_editor_T* s_sprite_editor)
{
    scene_sprite_editor_clear_all_frames(s_sprite_editor);

    if (s_sprite_editor->current_database_sprite != (void*) 0)
    {
        database_sprite_free(s_sprite_editor->current_database_sprite);
        s_sprite_editor->current_database_sprite = (void*) 0;
    }

    if (s_sprite_editor->sprite_id != (void*) 0)
    { 
        free(s_sprite_editor->sprite_id);
        s_sprite_editor->sprite_id = 0;
    }
}

void scene_sprite_editor_set_sprite_id(scene_sprite_editor_T* s_sprite_editor, char* id)
{
    if (s_sprite_editor->sprite_id)
        free(s_sprite_editor->sprite_id);

    char* id_new = calloc(strlen(id) + 1, sizeof(char));
    strcpy(id_new, id);

    s_sprite_editor->sprite_id = id_new;
}

void sprite_button_save_press()
{
    scene_T* scene = get_current_scene();
    scene_sprite_editor_T* s_sprite_editor = (scene_sprite_editor_T*) scene;

    if (!strlen(s_sprite_editor->input_field_name->value))
    {
        modal_manager_show_modal(MODAL_MANAGER, "error", "You need to enter a name.");

        return;
    }

    dynamic_list_T* textures = scene_sprite_editor_get_frames_as_textures(s_sprite_editor);

    float frame_delay = atof(s_sprite_editor->input_field_frame_delay->value);

    sprite_T* sprite = init_sprite(
        textures,
        frame_delay,
        s_sprite_editor->grid->width,
        s_sprite_editor->grid->height
    ); 

    if (s_sprite_editor->sprite_id == (void*)0)
    {
        printf("Inserting a new sprite %s.\n", s_sprite_editor->sprite_id);

        char* db_sprite_id = database_insert_sprite(
            DATABASE,
            s_sprite_editor->input_field_name->value,
            sprite
        );

        scene_sprite_editor_set_sprite_id(s_sprite_editor, db_sprite_id);
        database_sprite_T* database_sprite = database_get_sprite_by_id(DATABASE, db_sprite_id);
        s_sprite_editor->current_database_sprite = database_sprite;
    }
    else
    {
        printf("Updating existing sprite.\n");

        printf("FILEPATH: %s\n", s_sprite_editor->current_database_sprite->filepath);
        write_sprite_to_disk(sprite, s_sprite_editor->current_database_sprite->filepath);
        database_update_sprite_name_by_id(
            DATABASE,
            s_sprite_editor->current_database_sprite->id,
            s_sprite_editor->input_field_name->value
        );
    }

    REFRESH_STATE(s_sprite_editor);
}

void dropdown_list_sprite_press(void* dropdown_list, void* option)
{
    printf("Selected a sprite!\n");
    scene_T* scene = get_current_scene();
    scene_sprite_editor_T* s_sprite_editor = (scene_sprite_editor_T*) scene;

    scene_sprite_editor_delete_grids_reset_sprite_id(s_sprite_editor);

    dropdown_list_option_T* dropdown_list_option = (dropdown_list_option_T*) option;

    char* id = (char*) dropdown_list_option->value;

    database_sprite_T* database_sprite = database_get_sprite_by_id(DATABASE, id);
    sprite_T* sprite = database_sprite->sprite;

    s_sprite_editor->current_database_sprite = database_sprite;

    if (sprite == (void*)0)
    {
        scene_sprite_editor_clear_all_frames(s_sprite_editor);
    }
    else
    {
        scene_sprite_editor_load_grids_from_sprite(s_sprite_editor, sprite);
        scene_sprite_editor_refresh_grid(s_sprite_editor);
    }
    
    scene_sprite_editor_clear_input_fields(s_sprite_editor);

    s_sprite_editor->input_field_name->value = realloc(
        s_sprite_editor->input_field_name->value,
        (strlen(database_sprite->name) + 1) * sizeof(char)
    );
    
    strcpy(s_sprite_editor->input_field_name->value, database_sprite->name);

    if (sprite != (void*) 0)
    {
        char* frame_delay_str = calloc(128, sizeof(char));
        frame_delay_str[0] = '\0';
        sprintf(frame_delay_str, "%1.2f", sprite->frame_delay);

        s_sprite_editor->input_field_frame_delay->value = realloc(
            s_sprite_editor->input_field_frame_delay->value,
            (strlen(frame_delay_str) + 1) * sizeof(char)
        );

        strcpy(s_sprite_editor->input_field_frame_delay->value, frame_delay_str);
    }

    scene_sprite_editor_set_sprite_id(s_sprite_editor, database_sprite->id);
}

void sprite_button_new_press()
{
    printf("New!\n");

    scene_T* scene = get_current_scene();
    state_T* state = (state_T*) scene;
    scene_sprite_editor_T* s_sprite_editor = (scene_sprite_editor_T*) scene;
    
    if (!s_sprite_editor->sprite_id)
    {
        printf("You are not currently editing a sprite.\n");
        return;
    }

    database_sprite_free(s_sprite_editor->current_database_sprite);
    s_sprite_editor->current_database_sprite = (void*)0;

    memset(
        s_sprite_editor->input_field_name->value,
        '\0',
        strlen(s_sprite_editor->input_field_name->value) * sizeof(char)
    );

    s_sprite_editor->input_field_name->value = realloc(
        s_sprite_editor->input_field_name->value,
        1 * sizeof(char) 
    );

    s_sprite_editor->input_field_name->value[0] = '\0';

    scene_sprite_editor_delete_grids_reset_sprite_id(s_sprite_editor);

    // we need at least one grid in the state list.
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

    state_resort_actors(state);
}

void sprite_button_delete_press()
{
    scene_T* scene = get_current_scene();
    scene_sprite_editor_T* s_sprite_editor = (scene_sprite_editor_T*) scene;

    if (s_sprite_editor->sprite_id == (void*) 0)
    {
        printf("Not currently modifying a sprite.\n");
        return;
    }

    for (int i = 0; i < s_sprite_editor->dropdown_list_sprite->options->size; i++)
    {
        dropdown_list_option_T* option = s_sprite_editor->dropdown_list_sprite->options->items[i];

        if (option->value == (void*)0)
            continue;
        
        char* db_sprite_id = (char*) option->value;

        if (strcmp(db_sprite_id, s_sprite_editor->sprite_id) == 0)
        {
            dynamic_list_remove(
                s_sprite_editor->dropdown_list_sprite->options,
                option,
                _free_sprite_dropdown_option 
            );

            break;
        } 
    }

    database_delete_sprite_by_id(DATABASE, s_sprite_editor->sprite_id);

    printf("It was deleted\n");

    scene_sprite_editor_delete_grids_reset_sprite_id(s_sprite_editor);

    REFRESH_STATE(s_sprite_editor);
}

scene_sprite_editor_T* init_scene_sprite_editor()
{
    scene_sprite_editor_T* s_sprite_editor = calloc(1, sizeof(struct SCENE_SPRITE_EDITOR_STRUCT));
    scene_base_T* scene_base = (scene_base_T*) s_sprite_editor;
    scene_T* s = (scene_T*) scene_base;
    state_T* state = (state_T*) s; 

    scene_constructor(s, scene_sprite_editor_tick, scene_sprite_editor_draw, 2);
    scene_base_constructor(scene_base, scene_sprite_editor_refresh_state);

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
    REGISTER_FOCUSABLE(scene_base, s_sprite_editor->grid); 

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
    REGISTER_FOCUSABLE(scene_base, s_sprite_editor->grid_color_selector); 

    s_sprite_editor->tool_index = 0;

    s_sprite_editor->grid_tool_selector = init_grid(
        ((WINDOW_WIDTH / 2) - ((16 * 16) / 2)) + ((16 * 16) + 16) + (4 * 16) + 16,
        ((WINDOW_HEIGHT / 2) - ((16 * 16) / 2)),
        0.0f,
        1,
        2,
        16,
        0,
        0,
        0,
        "grid_tool_selector"
    );
    s_sprite_editor->grid_tool_selector->cells[0][0]->sprite = SPRITE_PENCIL;
    s_sprite_editor->grid_tool_selector->cells[0][1]->sprite = SPRITE_ERASOR;
    grid_unselect_all_cells(s_sprite_editor->grid_tool_selector);

    REGISTER_FOCUSABLE(scene_base, s_sprite_editor->grid_tool_selector);

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
            s_sprite_editor->grid_color_selector->cells[x][y]->a = 1.0f;
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
    REGISTER_FOCUSABLE(scene_base, s_sprite_editor->grid_color_mixer); 

    scene_base->focus_manager->focus_index = 0;

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
    REGISTER_FOCUSABLE(scene_base, s_sprite_editor->input_field_name);

    float button_width = 128;

    s_sprite_editor->button_save = init_button(
        (WINDOW_WIDTH / 2) - ((16 * 16) / 2),
        (WINDOW_HEIGHT / 2) + ((16 * 16) / 2) + 16,
        0.0f,
        "Save",
        sprite_button_save_press
    );
    s_sprite_editor->button_save->width = button_width;
    REGISTER_FOCUSABLE(scene_base, s_sprite_editor->button_save);

    s_sprite_editor->button_new = init_button(
        (WINDOW_WIDTH / 2) - ((16 * 16) / 2) + button_width + 16,
        (WINDOW_HEIGHT / 2) + ((16 * 16) / 2) + 16,
        0.0f,
        "New",
        sprite_button_new_press
    );
    s_sprite_editor->button_new->width = button_width;
    REGISTER_FOCUSABLE(scene_base, s_sprite_editor->button_new);

    s_sprite_editor->button_delete = init_button(
        (WINDOW_WIDTH / 2) - ((16 * 16) / 2) + button_width + button_width + 16 + 16,
        (WINDOW_HEIGHT / 2) + ((16 * 16) / 2) + 16,
        0.0f,
        "Delete",
        sprite_button_delete_press
    );
    s_sprite_editor->button_delete->width = button_width;
    REGISTER_FOCUSABLE(scene_base, s_sprite_editor->button_delete);
    
    int dropdown_list_sprite_width = 160;
    s_sprite_editor->label_current_sprite = init_label(
        ((WINDOW_WIDTH / 2) - ((16 * 16) / 2)) - (dropdown_list_sprite_width + 16),
        (WINDOW_HEIGHT / 2) - ((16 * 16) / 2) - (8 + 8),
        0.0f,
        "Sprite"
    );
    dynamic_list_append(state->actors, s_sprite_editor->label_current_sprite);

    s_sprite_editor->dropdown_list_sprite = init_dropdown_list(
        (((WINDOW_WIDTH / 2) - ((16 * 16) / 2))) - (dropdown_list_sprite_width + 16),
        (WINDOW_HEIGHT / 2) - ((16 * 16) / 2),
        0.0f,
        dropdown_list_sprite_press
    );
    s_sprite_editor->dropdown_list_sprite->expanded = 0;
    s_sprite_editor->dropdown_list_sprite->width = dropdown_list_sprite_width;
    ((actor_T*)s_sprite_editor->dropdown_list_sprite)->z = 1;
    REGISTER_FOCUSABLE(scene_base, (actor_focusable_T*)s_sprite_editor->dropdown_list_sprite);

    s_sprite_editor->label_frame_delay = init_label(
        (((WINDOW_WIDTH / 2) - ((16 * 16) / 2))) - (dropdown_list_sprite_width + 16),
        (WINDOW_HEIGHT / 2) - ((16 * 16) / 2) + 64,
        0.0f,
        "Delay"
    );
    dynamic_list_append(state->actors, s_sprite_editor->label_frame_delay);
    s_sprite_editor->input_field_frame_delay = init_input_field(
        (((WINDOW_WIDTH / 2) - ((16 * 16) / 2))) - (dropdown_list_sprite_width + 16),
        (WINDOW_HEIGHT / 2) - ((16 * 16) / 2) + 64 + 16,
        0.0f
    );
    s_sprite_editor->input_field_frame_delay->width = s_sprite_editor->dropdown_list_sprite->width;
    REGISTER_FOCUSABLE(scene_base, (actor_focusable_T*) s_sprite_editor->input_field_frame_delay);

    s_sprite_editor->grid_index = 0;
    s_sprite_editor->sprite_id = 0;
    s_sprite_editor->current_database_sprite = (void*) 0;

    s_sprite_editor->r = 0.0f;
    s_sprite_editor->g = 0.0f;
    s_sprite_editor->b = 0.0f;
    s_sprite_editor->a = 1.0f;

    state_resort_actors(state);

    REFRESH_STATE(s_sprite_editor);

    return s_sprite_editor;
}

void scene_sprite_editor_tick(scene_T* self)
{
    go_back_on_escape();

    scene_sprite_editor_T* s_sprite_editor = (scene_sprite_editor_T*) self;
    scene_base_T* scene_base = (scene_base_T*) s_sprite_editor;

    ((actor_focusable_T*)s_sprite_editor->button_delete)->visible = s_sprite_editor->sprite_id != (void*) 0;
    scene_base_tick(scene_base); 

    if (scene_base->focus_manager->focus_index == -1)
        return;

    actor_focusable_T* actor_focusable = (actor_focusable_T*) scene_base->focus_manager->focusables->items[scene_base->focus_manager->focus_index];
    actor_T* grid_actor = (actor_T*) actor_focusable;
    grid_T* grid = (grid_T*) grid_actor;
    actor_focusable->focused = 1;

    s_sprite_editor->grid_tool_selector->cells[0][s_sprite_editor->tool_index]->selected = 1;

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

    if (((actor_focusable_T*)s_sprite_editor->grid)->focused)
    {
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

            switch (s_sprite_editor->tool_index)
            {
                case 0: { // pencil tool
                    grid->cells[grid->cursor_x][grid->cursor_y]->r = s_sprite_editor->r;
                    grid->cells[grid->cursor_x][grid->cursor_y]->g = s_sprite_editor->g;
                    grid->cells[grid->cursor_x][grid->cursor_y]->b = s_sprite_editor->b;
                    grid->cells[grid->cursor_x][grid->cursor_y]->a = s_sprite_editor->a;
                } break;
                case 1: { // erasor tool
                    grid->cells[grid->cursor_x][grid->cursor_y]->r = 255;
                    grid->cells[grid->cursor_x][grid->cursor_y]->g = 255;
                    grid->cells[grid->cursor_x][grid->cursor_y]->b = 255;
                    grid->cells[grid->cursor_x][grid->cursor_y]->a = 0.0f;
                } break;
            }
        }

        if (s_sprite_editor->grids->size)
        {
            // save current state of the grid into the representation in our state list.
            grid_copy(
                s_sprite_editor->grid,
                s_sprite_editor->grids->items[s_sprite_editor->grid_index]
            );
        }

        if (KEYBOARD_STATE->keys[GLFW_KEY_1])
            scene_sprite_editor_set_tool(s_sprite_editor, 0); // pencil
        if (KEYBOARD_STATE->keys[GLFW_KEY_2])
            scene_sprite_editor_set_tool(s_sprite_editor, 1); // erasor
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
    else if (strcmp(grid_actor->type_name, "grid_tool_selector") == 0)
    {
        if (KEYBOARD_STATE->keys[GLFW_KEY_SPACE])
        {
            s_sprite_editor->tool_index = grid->cursor_y;
            scene_sprite_editor_set_tool(s_sprite_editor, s_sprite_editor->tool_index);
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
}

void scene_sprite_editor_draw(scene_T* self)
{
    state_T* state = (state_T*) self;
    scene_sprite_editor_T* s_sprite_editor = (scene_sprite_editor_T*) self;

    int number_of_frames = (int)(
        s_sprite_editor->grids->size > 0 ? s_sprite_editor->grids->size - 1 : 0
    );

    char grid_index_str[16];
    sprintf(grid_index_str, "%d / %d", (int)s_sprite_editor->grid_index, number_of_frames);

    draw_text(
        grid_index_str,
        ((actor_T*)s_sprite_editor->grid)->x + (16 / 2),
        ((actor_T*)s_sprite_editor->grid)->y - (16 / 2),
        0,
        0, // r
        0, // g
        0, // b
        1.0f, // a
        6,
        6,
        0,
        state
    );

    if (s_sprite_editor->sprite_id != (void*)0)
    {
        if (s_sprite_editor->current_database_sprite != (void*)0)
        {
            if (s_sprite_editor->current_database_sprite->sprite != (void*)0)
            {
                int spr_preview_size = 16;

                draw_positioned_sprite(
                    s_sprite_editor->current_database_sprite->sprite,
                    WINDOW_WIDTH - spr_preview_size - 8,
                    8,
                    0.0f,
                    spr_preview_size,
                    spr_preview_size,
                    state
                );
            }
        }
    }
}

void scene_sprite_editor_goto_next(scene_sprite_editor_T* self)
{
    self->grid_index += 1;
    int size = (int) self->grids->size;
    int index = (int) self->grid_index;

    if (size-1 < index)
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
    else
    {
        printf("No grid for you! %d %d\n", size-1, index);
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
        printf("Refreshing the grid is not possible since the frame list is empty.\n");
        grid_clean(self->grid);
        return;
    }
    
    grid_T* current_grid_state = (grid_T*) self->grids->items[self->grid_index]; 
    
    grid_clean(self->grid);

    printf("%d\n", (int)current_grid_state->width);
    grid_copy(current_grid_state, self->grid);
}

void scene_sprite_editor_load_grids_from_sprite(scene_sprite_editor_T* s_sprite_editor, sprite_T* sprite)
{
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

        for (y = 0; y < grid->height; y++)
        {
            for (x = 0; x < grid->width; x++)
            { 
                unsigned int channelCount = 4;
                unsigned char* pixelOffset = texture->data + (y * (int)grid->width + x) * channelCount;
                unsigned char r = pixelOffset[0];
                unsigned char g = pixelOffset[1];
                unsigned char b = pixelOffset[2];
                unsigned char a = pixelOffset[3];

                grid->cells[x][y]->r = r;
                grid->cells[x][y]->g = g;
                grid->cells[x][y]->b = b;
                grid->cells[x][y]->a = a;
            }
        }
        
        dynamic_list_append(
            s_sprite_editor->grids,
            grid 
        );
    }
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
