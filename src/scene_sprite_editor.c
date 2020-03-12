#include "include/main.h"
#include "include/scene_sprite_editor.h"
#include "include/etc.h"
#include "include/modal.h"
#include "include/modal_manager.h"
#include "include/constants.h"
#include <coelum/file_utils.h>
#include <coelum/constants.h>
#include <coelum/actor_text.h>
#include <coelum/input.h>
#include <coelum/utils.h>
#include <coelum/draw_utils.h>
#include <coelum/current.h>
#include <string.h>


extern keyboard_state_T* KEYBOARD_STATE;
extern mouse_state_T* MOUSE_STATE;
extern database_T* DATABASE;
extern main_state_T* MAIN_STATE;
extern modal_manager_T* MODAL_MANAGER;
extern sprite_T* SPRITE_PENCIL;
extern sprite_T* SPRITE_ERASOR;
extern const float COLOR_FG[3];


static void scene_sprite_editor_clear_component_input_fields(scene_sprite_editor_T* s_sprite_editor)
{
    memset(
        s_sprite_editor->component_input_field_name->value,
        '\0',
        strlen(s_sprite_editor->component_input_field_name->value) * sizeof(char)
    ); 

    memset(
        s_sprite_editor->component_input_field_frame_delay->value,
        '\0',
        strlen(s_sprite_editor->component_input_field_frame_delay->value) * sizeof(char)
    );
}

static void _free_sprite_component_dropdown_option(void* item)
{
    component_dropdown_list_option_T* component_dropdown_list_option = (component_dropdown_list_option_T*) item;
    
    free(component_dropdown_list_option->key);
    free((char*)component_dropdown_list_option->value);
    free(component_dropdown_list_option);
}

static void scene_sprite_editor_set_tool(scene_sprite_editor_T* s_sprite_editor, unsigned int tool_index)
{
    s_sprite_editor->tool_index = tool_index;
    component_grid_unselect_all_cells(s_sprite_editor->component_grid_tool_selector);
    s_sprite_editor->component_grid_tool_selector->cells[0][tool_index]->selected = 1;
}

void scene_sprite_editor_refresh_state(scene_base_T* scene_base)
{
    scene_sprite_editor_T* s_sprite_editor = (scene_sprite_editor_T*) scene_base;
    s_sprite_editor->component_grid->onion = (void*)0; // reset onion skin

    component_dropdown_list_sync_from_table(
        s_sprite_editor->component_dropdown_list_sprite,
        DATABASE,
        "sprites",
        1,
        0
    );

    component_dropdown_list_reload_sprites(s_sprite_editor->component_dropdown_list_sprite);

    if (s_sprite_editor->sprite_id != (void*) 0)
    {
        component_dropdown_list_set_selected_option_by_string_value(
            s_sprite_editor->component_dropdown_list_sprite,
            (const char*) s_sprite_editor->sprite_id 
        );
    }
    else
    {
        scene_sprite_editor_clear_component_input_fields(s_sprite_editor);
        scene_sprite_editor_clear_all_frames(s_sprite_editor);

        dynamic_list_append(
            s_sprite_editor->component_grids,
            init_component_grid(
                ((scene_base_T*)s_sprite_editor)->focus_manager,
                (RES_WIDTH / 2) - ((16 * 16) / 2),
                (RES_HEIGHT / 2) - ((16 * 16) / 2),
                0.0f,
                16,
                16,
                16,
                0,
                0,
                0,
                "component_grid_canvas"
            )
        );

        component_dropdown_list_reset(
            s_sprite_editor->component_dropdown_list_sprite        
        ); 
    }
}

void scene_sprite_editor_delete_component_grids_reset_sprite_id(scene_sprite_editor_T* s_sprite_editor)
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
        s_sprite_editor->sprite_id = (void*)0;

        printf("RESET!!!!!!!!!!!!!!!!!!!!!!!!\n");
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

void sprite_component_button_save_press()
{
    scene_T* scene = get_current_scene();
    scene_sprite_editor_T* s_sprite_editor = (scene_sprite_editor_T*) scene;

    if (!strlen(s_sprite_editor->component_input_field_name->value))
    {
        modal_manager_show_modal(MODAL_MANAGER, "error", "You need to enter a name.");

        return;
    }

    dynamic_list_T* textures = scene_sprite_editor_get_frames_as_textures(s_sprite_editor);

    float frame_delay = atof(s_sprite_editor->component_input_field_frame_delay->value);

    sprite_T* sprite = init_sprite(
        textures,
        frame_delay,
        s_sprite_editor->component_grid->width,
        s_sprite_editor->component_grid->height
    ); 

    if (s_sprite_editor->sprite_id == (void*)0)
    {
        printf("Inserting a new sprite %s.\n", s_sprite_editor->sprite_id);

        char* db_sprite_id = database_insert_sprite(
            DATABASE,
            s_sprite_editor->component_input_field_name->value,
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
            s_sprite_editor->component_input_field_name->value
        );
    }

    REFRESH_STATE(s_sprite_editor);
}

void component_dropdown_list_sprite_press(void* component_dropdown_list, void* option)
{
    printf("Selected a sprite!\n");
    scene_T* scene = get_current_scene();
    scene_sprite_editor_T* s_sprite_editor = (scene_sprite_editor_T*) scene;

    scene_sprite_editor_delete_component_grids_reset_sprite_id(s_sprite_editor);

    component_dropdown_list_option_T* component_dropdown_list_option = (component_dropdown_list_option_T*) option;

    char* id = (char*) component_dropdown_list_option->value;

    database_sprite_T* database_sprite = database_get_sprite_by_id(DATABASE, id);
    sprite_T* sprite = database_sprite->sprite;

    s_sprite_editor->current_database_sprite = database_sprite;

    if (sprite == (void*)0)
    {
        scene_sprite_editor_clear_all_frames(s_sprite_editor);
    }
    else
    {
        scene_sprite_editor_load_component_grids_from_sprite(s_sprite_editor, sprite);
        scene_sprite_editor_refresh_component_grid(s_sprite_editor);
    }
    
    scene_sprite_editor_clear_component_input_fields(s_sprite_editor);

    s_sprite_editor->component_input_field_name->value = realloc(
        s_sprite_editor->component_input_field_name->value,
        (strlen(database_sprite->name) + 1) * sizeof(char)
    );
    
    strcpy(s_sprite_editor->component_input_field_name->value, database_sprite->name);

    if (sprite != (void*) 0)
    {
        char* frame_delay_str = calloc(128, sizeof(char));
        frame_delay_str[0] = '\0';
        sprintf(frame_delay_str, "%1.2f", sprite->frame_delay);

        s_sprite_editor->component_input_field_frame_delay->value = realloc(
            s_sprite_editor->component_input_field_frame_delay->value,
            (strlen(frame_delay_str) + 1) * sizeof(char)
        );

        strcpy(s_sprite_editor->component_input_field_frame_delay->value, frame_delay_str);
    }

    scene_sprite_editor_set_sprite_id(s_sprite_editor, database_sprite->id);
}

void sprite_component_button_new_press()
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
        s_sprite_editor->component_input_field_name->value,
        '\0',
        strlen(s_sprite_editor->component_input_field_name->value) * sizeof(char)
    );

    s_sprite_editor->component_input_field_name->value = realloc(
        s_sprite_editor->component_input_field_name->value,
        1 * sizeof(char) 
    );

    s_sprite_editor->component_input_field_name->value[0] = '\0';

    scene_sprite_editor_delete_component_grids_reset_sprite_id(s_sprite_editor);

    // we need at least one component_grid in the state list.
    dynamic_list_append(
        s_sprite_editor->component_grids,
        init_component_grid(
            ((scene_base_T*)s_sprite_editor)->focus_manager,
            (RES_WIDTH / 2) - ((16 * 16) / 2),
            (RES_HEIGHT / 2) - ((16 * 16) / 2),
            0.0f,
            16,
            16,
            16,
            0,
            0,
            0,
            "component_grid_canvas"
        )
    ); 

    REFRESH_STATE(state);
}

void sprite_component_button_delete_press()
{
    scene_T* scene = get_current_scene();
    scene_sprite_editor_T* s_sprite_editor = (scene_sprite_editor_T*) scene;

    if (s_sprite_editor->sprite_id == (void*) 0)
    {
        printf("Not currently modifying a sprite.\n");
        return;
    }

    for (int i = 0; i < s_sprite_editor->component_dropdown_list_sprite->options->size; i++)
    {
        component_dropdown_list_option_T* option = s_sprite_editor->component_dropdown_list_sprite->options->items[i];

        if (option->value == (void*)0)
            continue;
        
        char* db_sprite_id = (char*) option->value;

        if (strcmp(db_sprite_id, s_sprite_editor->sprite_id) == 0)
        {
            dynamic_list_remove(
                s_sprite_editor->component_dropdown_list_sprite->options,
                option,
                _free_sprite_component_dropdown_option 
            );

            break;
        } 
    }

    database_delete_sprite_by_id(DATABASE, s_sprite_editor->sprite_id);

    scene_sprite_editor_delete_component_grids_reset_sprite_id(s_sprite_editor);

    REFRESH_STATE(s_sprite_editor);
}

scene_sprite_editor_T* init_scene_sprite_editor()
{
    scene_sprite_editor_T* s_sprite_editor = calloc(1, sizeof(struct SCENE_SPRITE_EDITOR_STRUCT));
    scene_base_T* scene_base = (scene_base_T*) s_sprite_editor;
    scene_T* s = (scene_T*) scene_base;
    state_T* state = (state_T*) s; 

    scene_constructor(s, scene_sprite_editor_tick, scene_sprite_editor_draw, 2);
    scene_base_constructor(scene_base, scene_sprite_editor_refresh_state, "Sprite Editor");

    s->type_name = "sprite_editor";
    s->bg_r = 255;
    s->bg_g = 255;
    s->bg_b = 255;

    float iy = 24.0f;

    s_sprite_editor->frame_index = calloc(1, sizeof(char));
    s_sprite_editor->frame_index[0] = '\0';

    component_pane_T* left = init_component_pane(state, scene_base->focus_manager, 0.0f, 0.0f, 0.0f, 0.0f);
    left->centered = 1;
    left->child_margin_top = COMPONENT_PANE_CHILD_MARGIN_TOP;
    component_pane_T* right = init_component_pane(state, scene_base->focus_manager, 0.0f, 0.0f, 0.0f, 0.0f);
    right->centered = 0;
    right->child_margin_top = 8;
    component_pane_T* right_top = init_component_pane(state, scene_base->focus_manager, 0.0f, 0.0f, 0.0f, 0.0f);
    right_top->centered = 1;
    right_top->min_height = RES_HEIGHT - 160;
    right_top->child_margin_top = COMPONENT_PANE_CHILD_MARGIN_TOP;
    component_pane_T* right_bottom = init_component_pane(state, scene_base->focus_manager, 0.0f, 0.0f, 0.0f, 0.0f);
    right_bottom->centered = 0;
    
    dynamic_list_append(scene_base->component_pane->cols, left);
    dynamic_list_append(scene_base->component_pane->cols, right);
    dynamic_list_append(right->rows, right_top);
    dynamic_list_append(right->rows, right_bottom);

    s_sprite_editor->component_grid = init_component_grid(
        scene_base->focus_manager,
        (RES_WIDTH / 2) - ((16 * 16) / 2),
        iy + (RES_HEIGHT / 2) - ((16 * 16) / 2),
        0.0f,
        16,
        16,
        16,
        0,
        0,
        0,
        "component_grid_canvas"
    );
    component_pane_add_component(
        right_top,
        (actor_component_T*) s_sprite_editor->component_grid        
    );

    s_sprite_editor->component_grids = init_dynamic_list(sizeof(struct GRID_STRUCT*));

    s_sprite_editor->component_grid_color_selector = init_component_grid(
        scene_base->focus_manager,
        ((RES_WIDTH / 2) - ((16 * 16) / 2)) + ((16 * 16) + 16),
        iy + ((RES_HEIGHT / 2) - ((16 * 16) / 2)),
        0.0f,
        4,
        8,
        16,
        0,
        0,
        0,
        "component_grid_color_selector"
    );
    component_pane_add_component(
        right_bottom,
        (actor_component_T*) s_sprite_editor->component_grid_color_selector 
    ); 

    // state representation of first component_grid in frame list.
    dynamic_list_append(
        s_sprite_editor->component_grids,
        init_component_grid(
            scene_base->focus_manager,
            (RES_WIDTH / 2) - ((16 * 16) / 2),
            iy + (RES_HEIGHT / 2) - ((16 * 16) / 2),
            0.0f,
            16,
            16,
            16,
            0,
            0,
            0,
            "component_grid_canvas"
        )
    );

    float r, g, b = 255.0f;

    for (int x = 0; x < s_sprite_editor->component_grid_color_selector->width; x++)
    {
        for (int y = 0; y < s_sprite_editor->component_grid_color_selector->height; y++)
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

            s_sprite_editor->component_grid_color_selector->cells[x][y]->r = r;
            s_sprite_editor->component_grid_color_selector->cells[x][y]->g = g;
            s_sprite_editor->component_grid_color_selector->cells[x][y]->b = b;
            s_sprite_editor->component_grid_color_selector->cells[x][y]->a = 1.0f;
        }
    }

    s_sprite_editor->component_grid_color_mixer = init_component_grid(
        scene_base->focus_manager,
        ((RES_WIDTH / 2) - ((16 * 16) / 2)) + ((16 * 16) + 16),
        iy + ((RES_HEIGHT / 2) - ((16 * 16) / 2)) + ((16 * 8) + 16),
        0.0f,
        8,
        7,
        16,
        0,
        0,
        0,
        "component_grid_color_mixer"
    );
    component_pane_add_component(
        right_bottom,
        (actor_component_T*) s_sprite_editor->component_grid_color_mixer
    );

    s_sprite_editor->tool_index = 0;

    s_sprite_editor->component_grid_tool_selector = init_component_grid(
        scene_base->focus_manager,
        ((RES_WIDTH / 2) - ((16 * 16) / 2)) + ((16 * 16) + 16) + (4 * 16) + 16,
        iy + ((RES_HEIGHT / 2) - ((16 * 16) / 2)),
        0.0f,
        1,
        2,
        16,
        0,
        0,
        0,
        "component_grid_tool_selector"
    );
    s_sprite_editor->component_grid_tool_selector->cells[0][0]->sprite = SPRITE_PENCIL;
    s_sprite_editor->component_grid_tool_selector->cells[0][1]->sprite = SPRITE_ERASOR;
    component_grid_unselect_all_cells(s_sprite_editor->component_grid_tool_selector);
    component_pane_add_component(
        right_bottom,
        (actor_component_T*) s_sprite_editor->component_grid_tool_selector
    );

    s_sprite_editor->component_label_frame_index = init_component_label(
        scene_base->focus_manager,
        0.0f, 0.0f, 0.0f,
        "0 / 0",
        COLOR_FG[0],
        COLOR_FG[1],
        COLOR_FG[2]
    );
    component_pane_add_component(
        right_top,
        (actor_component_T*) s_sprite_editor->component_label_frame_index
    );

    scene_base->focus_manager->focus_index = 0;

    s_sprite_editor->component_label_name = init_component_label(
        scene_base->focus_manager,
        (RES_WIDTH / 2) - ((16 * 16) / 2),
        iy + (RES_HEIGHT / 2) - ((16 * 16) / 2) - 80,
        0.0f,
        "Name",
        COLOR_FG[0],
        COLOR_FG[1],
        COLOR_FG[2]
    );
    component_pane_add_component(
        left,
        (actor_component_T*) s_sprite_editor->component_label_name
    );

    s_sprite_editor->component_input_field_name = init_component_input_field(
        scene_base->focus_manager,
        (RES_WIDTH / 2) - ((16 * 16) / 2),
        iy + (RES_HEIGHT / 2) - ((16 * 16) / 2) - 64,
        0.0f
    );
    component_pane_add_component(
        left,
        (actor_component_T*) s_sprite_editor->component_input_field_name
    );

    float component_button_width = 128;

    s_sprite_editor->component_button_save = init_component_button(
        scene_base->focus_manager,
        (RES_WIDTH / 2) - ((16 * 16) / 2),
        iy + (RES_HEIGHT / 2) + ((16 * 16) / 2) + 16,
        0.0f,
        "Save",
        sprite_component_button_save_press
    );
    component_pane_add_component(
        left,
        (actor_component_T*) s_sprite_editor->component_button_save
    );

    s_sprite_editor->component_button_new = init_component_button(
        scene_base->focus_manager,
        (RES_WIDTH / 2) - ((16 * 16) / 2) + component_button_width + 16,
        iy + (RES_HEIGHT / 2) + ((16 * 16) / 2) + 16,
        0.0f,
        "New",
        sprite_component_button_new_press
    );
    component_pane_add_component(
        left,
        (actor_component_T*) s_sprite_editor->component_button_new
    );

    s_sprite_editor->component_button_delete = init_component_button(
        scene_base->focus_manager,
        (RES_WIDTH / 2) - ((16 * 16) / 2) + component_button_width + component_button_width + 16 + 16,
        iy + (RES_HEIGHT / 2) + ((16 * 16) / 2) + 16,
        0.0f,
        "Delete",
        sprite_component_button_delete_press
    );
    component_pane_add_component(
        left,
        (actor_component_T*) s_sprite_editor->component_button_delete
    );
    
    int component_dropdown_list_sprite_width = 160;
    s_sprite_editor->component_label_current_sprite = init_component_label(
        scene_base->focus_manager,
        ((RES_WIDTH / 2) - ((16 * 16) / 2)) - (component_dropdown_list_sprite_width + 16),
        iy + (RES_HEIGHT / 2) - ((16 * 16) / 2) - (8 + 8),
        0.0f,
        "Sprite",
        COLOR_FG[0],
        COLOR_FG[1],
        COLOR_FG[2]
    );
    component_pane_add_component(
        left,
        (actor_component_T*) s_sprite_editor->component_label_current_sprite
    );

    s_sprite_editor->component_dropdown_list_sprite = init_component_dropdown_list(
        scene_base->focus_manager,
        (((RES_WIDTH / 2) - ((16 * 16) / 2))) - (component_dropdown_list_sprite_width + 16),
        iy + (RES_HEIGHT / 2) - ((16 * 16) / 2),
        0.0f,
        component_dropdown_list_sprite_press
    );
    s_sprite_editor->component_dropdown_list_sprite->expanded = 0;
    ((actor_T*)s_sprite_editor->component_dropdown_list_sprite)->z = 1;
    component_pane_add_component(
        left,
        (actor_component_T*) s_sprite_editor->component_dropdown_list_sprite
    );

    s_sprite_editor->component_label_frame_delay = init_component_label(
        scene_base->focus_manager,
        (((RES_WIDTH / 2) - ((16 * 16) / 2))) - (component_dropdown_list_sprite_width + 16),
        iy + (RES_HEIGHT / 2) - ((16 * 16) / 2) + 64,
        0.0f,
        "Delay",
        COLOR_FG[0],
        COLOR_FG[1],
        COLOR_FG[2]
    );
    component_pane_add_component(
        left,
        (actor_component_T*) s_sprite_editor->component_label_frame_delay
    );
    s_sprite_editor->component_input_field_frame_delay = init_component_input_field(
        scene_base->focus_manager,
        (((RES_WIDTH / 2) - ((16 * 16) / 2))) - (component_dropdown_list_sprite_width + 16),
        iy + (RES_HEIGHT / 2) - ((16 * 16) / 2) + 64 + 16,
        0.0f
    );
    component_pane_add_component(
        left,
        (actor_component_T*) s_sprite_editor->component_input_field_frame_delay
    );

    s_sprite_editor->component_grid_index = 0;
    s_sprite_editor->sprite_id = 0;
    s_sprite_editor->current_database_sprite = (void*) 0;

    s_sprite_editor->r = 0.0f;
    s_sprite_editor->g = 0.0f;
    s_sprite_editor->b = 0.0f;
    s_sprite_editor->a = 1.0f;
    
    scene_sprite_editor_set_tool(s_sprite_editor, s_sprite_editor->tool_index);

    state_resort_actors(state);

    REFRESH_STATE(s_sprite_editor);

    return s_sprite_editor;
}

void scene_sprite_editor_tick(scene_T* self)
{
    go_back_on_escape();

    scene_sprite_editor_T* s_sprite_editor = (scene_sprite_editor_T*) self;
    scene_base_T* scene_base = (scene_base_T*) s_sprite_editor;

    ((actor_focusable_T*)s_sprite_editor->component_button_delete)->visible = s_sprite_editor->sprite_id != (void*) 0;
    scene_base_tick(scene_base);

    if (s_sprite_editor->component_grid_index > 0)
    {
        s_sprite_editor->component_grid->onion = (component_grid_T*) s_sprite_editor->component_grids->items[s_sprite_editor->component_grid_index-1];
    }

    if (((actor_component_T*)s_sprite_editor->component_grid)->hovered)
    {
        component_grid_T* component_grid = s_sprite_editor->component_grid;

        component_grid->cursor_x = (int)((int)((MOUSE_STATE->x - ((actor_T*)component_grid)->x) / component_grid->cell_size) % (int)component_grid->width);
        component_grid->cursor_y = (int)((int)((MOUSE_STATE->y - ((actor_T*)component_grid)->y) / component_grid->cell_size) % (int)component_grid->height);

        if (MOUSE_STATE->button_left)
        {
            if (
               component_grid->cursor_x < 0 ||
               component_grid->cursor_x > component_grid->width - 1 ||
               component_grid->cursor_y < 0 ||
               component_grid->cursor_y > component_grid->height - 1
            )
                return;

            switch (s_sprite_editor->tool_index)
            {
                case 0: { // pencil tool
                    component_grid->cells[component_grid->cursor_x][component_grid->cursor_y]->r = s_sprite_editor->r;
                    component_grid->cells[component_grid->cursor_x][component_grid->cursor_y]->g = s_sprite_editor->g;
                    component_grid->cells[component_grid->cursor_x][component_grid->cursor_y]->b = s_sprite_editor->b;
                    component_grid->cells[component_grid->cursor_x][component_grid->cursor_y]->a = s_sprite_editor->a;
                } break;
                case 1: { // erasor tool
                    component_grid->cells[component_grid->cursor_x][component_grid->cursor_y]->r = 255;
                    component_grid->cells[component_grid->cursor_x][component_grid->cursor_y]->g = 255;
                    component_grid->cells[component_grid->cursor_x][component_grid->cursor_y]->b = 255;
                    component_grid->cells[component_grid->cursor_x][component_grid->cursor_y]->a = 0.0f;
                } break;
            }
        }

        if (s_sprite_editor->component_grids->size)
        {
            // save current state of the component_grid into the representation in our state list.
            component_grid_copy(
                s_sprite_editor->component_grid,
                s_sprite_editor->component_grids->items[s_sprite_editor->component_grid_index]
            );
        }

        if (KEYBOARD_STATE->keys[GLFW_KEY_1])
            scene_sprite_editor_set_tool(s_sprite_editor, 0); // pencil
        if (KEYBOARD_STATE->keys[GLFW_KEY_2])
            scene_sprite_editor_set_tool(s_sprite_editor, 1); // erasor

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
    }
    else // mixer & color selector
    if (
        ((actor_component_T*)s_sprite_editor->component_grid_color_selector)->hovered ||
        ((actor_component_T*)s_sprite_editor->component_grid_color_mixer)->hovered
    )
    {
        component_grid_T* component_grid = (void*)0;

        if (((actor_component_T*)s_sprite_editor->component_grid_color_selector)->hovered)
            component_grid = s_sprite_editor->component_grid_color_selector;
        else
            component_grid = s_sprite_editor->component_grid_color_mixer;

        component_grid->cursor_x = (int)((int)((MOUSE_STATE->x - ((actor_T*)component_grid)->x) / component_grid->cell_size) % (int)component_grid->width);
        component_grid->cursor_y = (int)((int)((MOUSE_STATE->y - ((actor_T*)component_grid)->y) / component_grid->cell_size) % (int)component_grid->height);

        if (MOUSE_STATE->button_left)
        {
            if (
               component_grid->cursor_x < 0 ||
               component_grid->cursor_x > component_grid->width - 1 ||
               component_grid->cursor_y < 0 ||
               component_grid->cursor_y > component_grid->height - 1
            )
                return;

            s_sprite_editor->r = component_grid->cells[component_grid->cursor_x][component_grid->cursor_y]->r;
            s_sprite_editor->g = component_grid->cells[component_grid->cursor_x][component_grid->cursor_y]->g;
            s_sprite_editor->b = component_grid->cells[component_grid->cursor_x][component_grid->cursor_y]->b;
        }
    }
    else if (((actor_component_T*)s_sprite_editor->component_grid_tool_selector)->hovered)
    {
        component_grid_T* component_grid = s_sprite_editor->component_grid_tool_selector;

        component_grid->cursor_x = (int)((int)((MOUSE_STATE->x - ((actor_T*)component_grid)->x) / component_grid->cell_size) % (int)component_grid->width);
        component_grid->cursor_y = (int)((int)((MOUSE_STATE->y - ((actor_T*)component_grid)->y) / component_grid->cell_size) % (int)component_grid->height);

        if (MOUSE_STATE->button_left)
        {
            scene_sprite_editor_set_tool(s_sprite_editor, component_grid->cursor_y);
        }
    }

    if (((actor_component_T*)s_sprite_editor->component_grid_color_mixer)->hovered)
    {
        component_grid_T* component_grid = s_sprite_editor->component_grid_color_mixer;

        if (MOUSE_STATE->button_right)
        {
            if (
               component_grid->cursor_x < 0 ||
               component_grid->cursor_x > component_grid->width - 1 ||
               component_grid->cursor_y < 0 ||
               component_grid->cursor_y > component_grid->height - 1
            )
                return;

            if (
                component_grid->cells[component_grid->cursor_x][component_grid->cursor_y]->r == 255 &&
                component_grid->cells[component_grid->cursor_x][component_grid->cursor_y]->g == 255 &&
                component_grid->cells[component_grid->cursor_x][component_grid->cursor_y]->b == 255
            )
            {
                component_grid->cells[component_grid->cursor_x][component_grid->cursor_y]->r = 0;
                component_grid->cells[component_grid->cursor_x][component_grid->cursor_y]->g = 0;
                component_grid->cells[component_grid->cursor_x][component_grid->cursor_y]->b = 0;
            }

            component_grid->cells[component_grid->cursor_x][component_grid->cursor_y]->r = (s_sprite_editor->r + component_grid->cells[component_grid->cursor_x][component_grid->cursor_y]->r);
            component_grid->cells[component_grid->cursor_x][component_grid->cursor_y]->g = (s_sprite_editor->g + component_grid->cells[component_grid->cursor_x][component_grid->cursor_y]->g);
            component_grid->cells[component_grid->cursor_x][component_grid->cursor_y]->b = (s_sprite_editor->b + component_grid->cells[component_grid->cursor_x][component_grid->cursor_y]->b);
            component_grid->cells[component_grid->cursor_x][component_grid->cursor_y]->a = 255;
        }
    }
}

void scene_sprite_editor_draw(scene_T* self)
{
    scene_sprite_editor_T* s_sprite_editor = (scene_sprite_editor_T*) self;

    scene_base_draw((scene_base_T*) s_sprite_editor);

    int number_of_frames = (int)(
        s_sprite_editor->component_grids->size > 0 ? s_sprite_editor->component_grids->size - 1 : 0
    );

    const char* frame_index_template = "%d / %d";
    s_sprite_editor->frame_index = realloc(s_sprite_editor->frame_index, (strlen(frame_index_template) + 256) * sizeof(char));
    sprintf(s_sprite_editor->frame_index, frame_index_template, (int)s_sprite_editor->component_grid_index, number_of_frames);
    s_sprite_editor->component_label_frame_index->text = s_sprite_editor->frame_index;
}

void scene_sprite_editor_goto_next(scene_sprite_editor_T* self)
{
    self->component_grid_index += 1;
    int size = (int) self->component_grids->size;
    int index = (int) self->component_grid_index;

    if (size-1 < index)
    {
        printf("Created another component_grid state representation!\n");

        dynamic_list_append(
            self->component_grids,
            init_component_grid(
                ((scene_base_T*)self)->focus_manager,
                (RES_WIDTH / 2) - ((16 * 16) / 2),
                (RES_HEIGHT / 2) - ((16 * 16) / 2),
                0.0f,
                16,
                16,
                16,
                0,
                0,
                0,
                "component_grid_canvas"
            )
        );
    }
    else
    {
        printf("No component_grid for you! %d %d\n", size-1, index);
    }

    scene_sprite_editor_refresh_component_grid(self);
}

void scene_sprite_editor_goto_prev(scene_sprite_editor_T* self)
{
    if (self->component_grid_index > 0)
        self->component_grid_index -= 1;

    scene_sprite_editor_refresh_component_grid(self);
}

void _component_grid_free(void* item)
{
    component_grid_free((component_grid_T*) item);
}

void scene_sprite_editor_delete_current_frame(scene_sprite_editor_T* self)
{
    if (self->component_grid_index == 0)
    {
        printf("Cannot delete first frame\n");
        return;
    }
    
    component_grid_T* current_component_grid_state = (component_grid_T*) self->component_grids->items[self->component_grid_index];

    dynamic_list_remove(self->component_grids, current_component_grid_state, _component_grid_free);

    self->component_grid_index -= 1;

    scene_sprite_editor_refresh_component_grid(self);
}

void scene_sprite_editor_clear_all_frames(scene_sprite_editor_T* self)
{
    for (int i = self->component_grids->size; i > 0; i--)
    {
        component_grid_T* component_grid_state = (component_grid_T*) self->component_grids->items[i];

        dynamic_list_remove(self->component_grids, component_grid_state, _component_grid_free);

        self->component_grid_index -= 1;
    }

    self->component_grid_index = 0;

    scene_sprite_editor_refresh_component_grid(self);
}

void scene_sprite_editor_refresh_component_grid(scene_sprite_editor_T* self)
{
    if (self->component_grids->size == 0)
    {
        printf("Refreshing the component_grid is not possible since the frame list is empty.\n");
        component_grid_clean(self->component_grid);
        return;
    }
    
    component_grid_T* current_component_grid_state = (component_grid_T*) self->component_grids->items[self->component_grid_index]; 
    
    component_grid_clean(self->component_grid);

    component_grid_copy(current_component_grid_state, self->component_grid);
}

void scene_sprite_editor_load_component_grids_from_sprite(scene_sprite_editor_T* s_sprite_editor, sprite_T* sprite)
{
    for (int i = 0; i < sprite->textures->size; i++)
    {
        texture_T* texture = (texture_T*) sprite->textures->items[i];

        component_grid_T* component_grid = init_component_grid(
            ((scene_base_T*)s_sprite_editor)->focus_manager,
            (RES_WIDTH / 2) - ((16 * 16) / 2),
            (RES_HEIGHT / 2) - ((16 * 16) / 2),
            0.0f,
            16,
            16,
            16,
            0,
            0,
            0,
            "component_grid_canvas"
        );

        int y, x;

        for (y = 0; y < component_grid->height; y++)
        {
            for (x = 0; x < component_grid->width; x++)
            { 
                unsigned int channelCount = 4;
                unsigned char* pixelOffset = texture->data + (y * (int)component_grid->width + x) * channelCount;
                unsigned char r = pixelOffset[0];
                unsigned char g = pixelOffset[1];
                unsigned char b = pixelOffset[2];
                unsigned char a = pixelOffset[3];

                component_grid->cells[x][y]->r = r;
                component_grid->cells[x][y]->g = g;
                component_grid->cells[x][y]->b = b;
                component_grid->cells[x][y]->a = a;
            }
        }
        
        dynamic_list_append(
            s_sprite_editor->component_grids,
            component_grid 
        );
    }
}

dynamic_list_T* scene_sprite_editor_get_frames_as_textures(scene_sprite_editor_T* self)
{
    dynamic_list_T* frame_list = init_dynamic_list(sizeof(struct TEXTURE_STRUCT*));

    for (int i = 0; i < self->component_grids->size; i++)
    {
        component_grid_T* component_grid = (component_grid_T*) self->component_grids->items[i];

        dynamic_list_append(frame_list, component_grid_create_texture(component_grid));
    }

    return frame_list;
}
