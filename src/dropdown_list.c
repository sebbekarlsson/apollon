#include "include/dropdown_list.h"
#include <coelum/file_utils.h>
#include <coelum/theatre.h>
#include <coelum/draw_utils.h>
#include <coelum/input.h>
#include <string.h>
#include <spr/spr.h>


extern theatre_T* THEATRE;
extern keyboard_state_T* KEYBOARD_STATE;

extern float COLOR_BG_DARK[3];
extern float COLOR_BG_DARK_BRIGHT[3];
extern float COLOR_FG[3];

dropdown_list_T* init_dropdown_list(float x, float y, float z, void (*press)(void* dropdown_list, void* option))
{
    dropdown_list_T* dropdown_list = calloc(1, sizeof(struct DROPDOWN_LIST_STRUCT));
    actor_T* actor = (actor_T*) dropdown_list;
    actor_constructor(actor, x, y, z, dropdown_list_tick, dropdown_list_draw, "dropdown_list");
    actor_focusable_T* actor_focusable = (actor_focusable_T*) actor;
    actor_focusable_constructor(actor_focusable);
    
    dropdown_list->options = init_dynamic_list(sizeof(struct DROPDOWN_LIST_OPTION_STRUCT*));
    dropdown_list->option_index = 0;
    dropdown_list->expanded = 0;
    dropdown_list->selected_index = 0;
    dropdown_list->width = 200;
    dropdown_list->press = press;

    return dropdown_list;
}

void dropdown_list_tick(actor_T* self)
{
    dropdown_list_T* dropdown_list = (dropdown_list_T*) self;
    actor_focusable_T* actor_focusable = (actor_focusable_T*) dropdown_list;

    if (!actor_focusable->focused)
    {
        dropdown_list->expanded = 0;
        return;
    }
    else
    {
        dropdown_list->expanded = 1;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_UP] && !KEYBOARD_STATE->key_locks[GLFW_KEY_UP])
    {
        if (dropdown_list->option_index > 0)
            dropdown_list->option_index -= 1;

        KEYBOARD_STATE->key_locks[GLFW_KEY_UP] = 1;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_DOWN] && !KEYBOARD_STATE->key_locks[GLFW_KEY_DOWN])
    {
        if (dropdown_list->option_index < dropdown_list->options->size - 1)
            dropdown_list->option_index += 1;

        KEYBOARD_STATE->key_locks[GLFW_KEY_DOWN] = 1;
    }
    
    if (KEYBOARD_STATE->keys[GLFW_KEY_ENTER] && !KEYBOARD_STATE->key_locks[GLFW_KEY_ENTER])
    {
        dropdown_list->selected_index = dropdown_list->option_index;
        KEYBOARD_STATE->key_locks[GLFW_KEY_ENTER] = 1;

        if (dropdown_list->press && dropdown_list->options->size > 0)
        {
            dropdown_list->press(dropdown_list, dropdown_list->options->items[dropdown_list->option_index]);
        }
    }
}

void dropdown_list_option_draw(int i, dropdown_list_option_T* option, dropdown_list_T* dropdown_list, state_T* state)
{
    actor_focusable_T* actor_focusable = (actor_focusable_T*) dropdown_list;
    actor_T* self = (actor_T*) actor_focusable;

    if ((i == dropdown_list->option_index || i == dropdown_list->selected_index) && actor_focusable->focused)
    {
        draw_positioned_2D_mesh(
            self->x,
            (self->y + (i * 32)),
            0.0f,
            dropdown_list->width,
            32,
            COLOR_BG_DARK_BRIGHT[0],
            COLOR_BG_DARK_BRIGHT[1],
            COLOR_BG_DARK_BRIGHT[2],
            state
        );
    }

    int text_padding = 0;

    if (option->database_sprite)
    {
        if (option->database_sprite->sprite)
        {
            draw_positioned_sprite(
                option->database_sprite->sprite,
                self->x + 4,
                (self->y + (i * 32)) + 8,
                0.0f,
                16,
                16,
                state
            );

            text_padding = 16;
        }
    }

    if (option->key)
    {
        if (strlen(option->key))
        {
            draw_text(
                option->key,
                self->x + (32 / 2) + text_padding,
                (self->y + (i * 32)) + (32 / 2),
                0,
                COLOR_FG[0], // r
                COLOR_FG[1], // g
                COLOR_FG[2], // b
                6,
                6,
                option->text_limit,
                state
            );
        }
    }
}

void dropdown_list_draw(actor_T* self)
{
    dropdown_list_T* dropdown_list = (dropdown_list_T*) self;
    actor_focusable_T* dropdown_list_focusable = (actor_focusable_T*) dropdown_list;
    unsigned int focused = dropdown_list_focusable->focused;

    if (!dropdown_list_focusable->visible)
        return;

    scene_T* scene = scene_manager_get_current_scene(THEATRE->scene_manager);
    state_T* state = (state_T*) scene;

    draw_positioned_2D_mesh(
        self->x,
        self->y,
        0.0f,
        dropdown_list->width,
        32 * (dropdown_list->expanded && dropdown_list->options->size > 0 ? dropdown_list->options->size : 1),
        COLOR_BG_DARK[0],
        COLOR_BG_DARK[1],
        COLOR_BG_DARK[2],
        state
    ); 

    if (dropdown_list->expanded && dropdown_list->options->size > 0)
    {
        for (int i = 0; i < dropdown_list->options->size; i++)
        {
            dropdown_list_option_T* option = (dropdown_list_option_T*) dropdown_list->options->items[i];

            dropdown_list_option_draw(i, option, dropdown_list, state);
        }
    }
    else
    {
        if (dropdown_list->options->size > 0)
        {
            for (int i = 0; i < dropdown_list->options->size; i++)
            {
                dropdown_list_option_T* option = (dropdown_list_option_T*) dropdown_list->options->items[i];

                if (i == dropdown_list->selected_index)
                {
                    dropdown_list_option_draw(0, option, dropdown_list, state);
                    break;
                }
            }
        }
        else
        {
            draw_text(
                "EMPTY",
                self->x + (32 / 2),
                (self->y + (32 / 2)),
                0,
                focused ? 255 : COLOR_FG[0], // r
                focused ? 255 : COLOR_FG[1], // g
                focused ? 255 : COLOR_FG[2], // b
                6,
                6,
                0,
                state
            );
        }
    }
}

dropdown_list_option_T* init_dropdown_list_option(database_sprite_T* database_sprite, char* key, void* value, unsigned int text_limit)
{
    dropdown_list_option_T* option = calloc(1, sizeof(struct DROPDOWN_LIST_OPTION_STRUCT));
    option->database_sprite = database_sprite;
    option->key = key;
    option->value = value;
    option->text_limit = text_limit;

    return option;
}

void dropdown_list_set_selected_option_by_string_value(dropdown_list_T* dropdown_list, const char* value)
{
    for (int i = 0; i < dropdown_list->options->size; i++)
    {
        dropdown_list_option_T* dropdown_list_option = (dropdown_list_option_T*) dropdown_list->options->items[i];

        if (strcmp((char*)dropdown_list_option->value, value) == 0)
        {
            dropdown_list->selected_index = i;
            break;
        }
    }
}

unsigned int dropdown_list_has_option_with_string_value(dropdown_list_T* dropdown_list, const char* value)
{
    for (int i = 0; i < dropdown_list->options->size; i++)
    {
        dropdown_list_option_T* dropdown_list_option = (dropdown_list_option_T*) dropdown_list->options->items[i];

        if (strcmp((char*)dropdown_list_option->value, value) == 0)
            return 1;
    }

    return 0;
}

void dropdown_list_update_option_sprite_by_string_value(
    dropdown_list_T* dropdown_list,
    const char* value,
    database_sprite_T* database_sprite
)
{
    for (int i = 0; i < dropdown_list->options->size; i++)
    {
        dropdown_list_option_T* dropdown_list_option = (dropdown_list_option_T*) dropdown_list->options->items[i];

        if (strcmp((char*)dropdown_list_option->value, value) == 0)
        {
            database_sprite_free(dropdown_list_option->database_sprite);
            dropdown_list_option->database_sprite = database_sprite;

            break;
        }
    }
}

void dropdown_list_update_option_key_by_string_value(
    dropdown_list_T* dropdown_list,
    const char* value,
    char* key        
)
{
    for (int i = 0; i < dropdown_list->options->size; i++)
    {
        dropdown_list_option_T* dropdown_list_option = (dropdown_list_option_T*) dropdown_list->options->items[i];

        if (strcmp((char*)dropdown_list_option->value, value) == 0)
        {
            free(dropdown_list_option->key);
            dropdown_list_option->key = key;

            break;
        }
    }
}

void dropdown_list_sync_from_table(dropdown_list_T* dropdown_list, database_T* database, const char* tablename, unsigned int key_column, int sprite_id_column)
{
    char* sql_template = "SELECT * FROM %s";
    char* sql = calloc(strlen(sql_template) + strlen(tablename) + 1, sizeof(char));
    sprintf(sql, sql_template, tablename);

    sqlite3_stmt* stmt = database_exec_sql(database, sql, 0);

	while (sqlite3_step(stmt) != SQLITE_DONE) {
        const unsigned char* _value = sqlite3_column_text(stmt, 0);
        const unsigned char* _key = sqlite3_column_text(stmt, key_column);

        char* value = calloc(strlen((char*) _value) + 1, sizeof(char));
        strcpy(value, _value);

        char* key = calloc(strlen((char*) _key) + 1, sizeof(char));
        strcpy(key,  _key); 

        // loading serialized sprite
        const unsigned char* sprite_id = 0;
        database_sprite_T* database_sprite = (void*) 0;

        if (sprite_id_column != -1)
        {
            sprite_id = sqlite3_column_text(stmt, sprite_id_column);
            database_sprite = database_get_sprite_by_id(database, sprite_id);
        }

        if (dropdown_list_has_option_with_string_value(dropdown_list, value))
        {
            if (sprite_id_column != -1)
            {
                printf("Update dropdown_list_option.\n");
                dropdown_list_update_option_sprite_by_string_value(
                    dropdown_list,
                    (const char*) value,
                    database_sprite        
                );
            }

            dropdown_list_update_option_key_by_string_value(
                dropdown_list,
                (const char*) value,
                key        
            );
        }
        else
        {
            dynamic_list_append(
                dropdown_list->options,
                init_dropdown_list_option(
                    database_sprite,
                    key,
                    value,
                    0
                )
            );
        }
	}

	sqlite3_finalize(stmt);

	sqlite3_close(database->db);
}

void dropdown_list_reload_sprites(dropdown_list_T* dropdown_list)
{
    for (int i = 0; i < dropdown_list->options->size; i++)
    {
        dropdown_list_option_T* dropdown_list_option = (dropdown_list_option_T*) dropdown_list->options->items[i];
        database_sprite_reload_from_disk(dropdown_list_option->database_sprite);
    }
}

dropdown_list_option_T* dropdown_list_get_selected_option(dropdown_list_T* dropdown_list)
{
    for (int i = 0; i < dropdown_list->options->size; i++)
    {
        dropdown_list_option_T* dropdown_list_option = (dropdown_list_option_T*) dropdown_list->options->items[i];
        if (i == dropdown_list->selected_index)
        {
            return dropdown_list_option;
        }
    }

    return (void*) 0;
}
