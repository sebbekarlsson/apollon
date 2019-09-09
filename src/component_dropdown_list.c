#include "include/component_dropdown_list.h"
#include <coelum/file_utils.h>
#include <coelum/theatre.h>
#include <coelum/draw_utils.h>
#include <coelum/input.h>
#include <string.h>
#include <spr/spr.h>


extern theatre_T* THEATRE;
extern mouse_state_T* MOUSE_STATE;
extern sprite_T* SPRITE_ARROW_DOWN;

extern float COLOR_BG_DARK[3];
extern float COLOR_BG_DARK_BRIGHT[3];
extern float COLOR_FG[3];

static void dropdown_click(actor_T* self)
{
    component_dropdown_list_T* component_dropdown_list = (component_dropdown_list_T*) self;
    component_dropdown_list->selected_index = component_dropdown_list->option_index;
    
    if (component_dropdown_list->press && component_dropdown_list->options->size > 0 && component_dropdown_list->option_index > 0)
    {
        component_dropdown_list->press(component_dropdown_list, component_dropdown_list->options->items[component_dropdown_list->option_index]);
    }

    if (component_dropdown_list->deexpand_on_press)
    {
        component_dropdown_list->expanded = 0;
        component_dropdown_list->focused = 0;
    }
}

component_dropdown_list_T* init_component_dropdown_list(focus_manager_T* focus_manager, float x, float y, float z, void (*press)(void* component_dropdown_list, void* option))
{
    component_dropdown_list_T* component_dropdown_list = calloc(1, sizeof(struct COMPONENT_DROPDOWN_LIST_STRUCT));
    actor_T* actor = (actor_T*) component_dropdown_list;
    actor_constructor(actor, x, y, z, component_dropdown_list_tick, component_dropdown_list_draw, "component_dropdown_list");
    actor_component_T* actor_component = (actor_component_T*) actor;
    actor_component_constructor(actor_component, focus_manager, dropdown_click);
    
    component_dropdown_list->options = init_dynamic_list(sizeof(struct COMPONENT_DROPDOWN_LIST_OPTION_STRUCT*));
    dynamic_list_append(
        component_dropdown_list->options,
        init_component_dropdown_list_option((void*)0, "No Option", (void*)0, 0)
    );

    component_dropdown_list->option_index = 0;
    component_dropdown_list->expanded = 0;
    component_dropdown_list->selected_index = 0;
    component_dropdown_list->deexpand_on_press = 1;
    actor->width = 200;
    actor->height = 28;
    component_dropdown_list->press = press;

    return component_dropdown_list;
}

void component_dropdown_list_tick(actor_T* self)
{
    component_dropdown_list_T* component_dropdown_list = (component_dropdown_list_T*) self;
    actor_component_T* actor_component = (actor_component_T*) component_dropdown_list;

    actor_component->prio = component_dropdown_list->expanded;

    actor_component_tick(actor_component);

    if (!actor_component->focused)
    {
        component_dropdown_list->expanded = 0;
        self->height = component_dropdown_list->expanded ? component_dropdown_list->options->size * 28 : 28;
        return;
    }
    else
    {
        component_dropdown_list->expanded = 1;
        self->height = component_dropdown_list->expanded ? component_dropdown_list->options->size * 28 : 28;
    }


    for (int i = 0; i < component_dropdown_list->options->size; i++)
    {
        component_dropdown_list_option_T* option = (component_dropdown_list_option_T*) component_dropdown_list->options->items[i];

        int x = self->x;
        int y = self->y + (i * 28);

        if (MOUSE_STATE->x >= x && MOUSE_STATE->x <= x + self->width)
        {
            if (MOUSE_STATE->y >= y && MOUSE_STATE->y <= y + 28)
            {
                component_dropdown_list->option_index = i;
            }
        }
    }
}

void component_dropdown_list_option_draw(int i, component_dropdown_list_option_T* option, component_dropdown_list_T* component_dropdown_list, state_T* state)
{
    actor_component_T* actor_component = (actor_component_T*) component_dropdown_list;
    actor_T* self = (actor_T*) actor_component;

    int h = 28;

    float base_z = self->z;

    if (component_dropdown_list->expanded)
        base_z += 0.8f;

    if ((i == component_dropdown_list->option_index || i == component_dropdown_list->selected_index) && actor_component->focused)
    {
        draw_positioned_2D_mesh(
            self->x,
            (self->y + (i * h)),
            base_z,
            self->width,
            h,
            COLOR_BG_DARK_BRIGHT[0],
            COLOR_BG_DARK_BRIGHT[1],
            COLOR_BG_DARK_BRIGHT[2],
            1.0f,
            state
        );
    }

    int text_padding = 0;
    
    if (option->database_sprite != (void*) 0)
    {
        if (option->database_sprite->sprite != (void*) 0)
        {
            draw_positioned_sprite(
                option->database_sprite->sprite,
                self->x + 4,
                self->y + (i * h) + ((h/2) - 8),
                base_z + 0.9f,
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
                self->x + (h / 2) + text_padding,
                (self->y + (i * h)) + (h / 2),
                base_z + 0.2f,
                COLOR_FG[0], // r
                COLOR_FG[1], // g
                COLOR_FG[2], // b
                1.0f, // a
                6,
                6,
                option->text_limit,
                state
            );
        }
    }
}

void component_dropdown_list_draw(actor_T* self)
{
    component_dropdown_list_T* component_dropdown_list = (component_dropdown_list_T*) self;
    actor_component_T* component_dropdown_list_component = (actor_component_T*) component_dropdown_list;
    unsigned int focused = component_dropdown_list_component->focused;

    if (!component_dropdown_list_component->visible)
        return;

    scene_T* scene = scene_manager_get_current_scene(THEATRE->scene_manager);
    state_T* state = (state_T*) scene;

    int h = 28;

    float base_z = self->z;

    if (component_dropdown_list->expanded)
        base_z += 0.3f;

    if (component_dropdown_list->expanded)
    {
        draw_positioned_2D_mesh(
            self->x + 4,
            self->y + 4,
            base_z + 0.1f,
            self->width,
            h * (component_dropdown_list->expanded && component_dropdown_list->options->size > 0 ? component_dropdown_list->options->size : 1),
            0,
            0,
            0,
            0.6f,
            state
        );
    } 

    draw_positioned_2D_mesh(
        self->x,
        self->y,
        base_z + 0.4f,
        self->width,
        h * (component_dropdown_list->expanded && component_dropdown_list->options->size > 0 ? component_dropdown_list->options->size : 1),
        COLOR_BG_DARK[0],
        COLOR_BG_DARK[1],
        COLOR_BG_DARK[2],
        1.0f,
        state
    ); 

    if (component_dropdown_list->expanded && component_dropdown_list->options->size > 0)
    {
        for (int i = 0; i < component_dropdown_list->options->size; i++)
        {
            component_dropdown_list_option_T* option = (component_dropdown_list_option_T*) component_dropdown_list->options->items[i];

            component_dropdown_list_option_draw(i, option, component_dropdown_list, state);
        }
    }
    else
    {
        if (component_dropdown_list->options->size > 0)
        {
            for (int i = 0; i < component_dropdown_list->options->size; i++)
            {
                component_dropdown_list_option_T* option = (component_dropdown_list_option_T*) component_dropdown_list->options->items[i];

                if (i == component_dropdown_list->selected_index)
                {
                    component_dropdown_list_option_draw(0, option, component_dropdown_list, state);
                    break;
                }
            }
        }
        else
        {
            draw_text(
                "EMPTY",
                self->x + (h / 2),
                (self->y + (h / 2)),
                base_z + 0.1f,
                focused ? 255 : COLOR_FG[0], // r
                focused ? 255 : COLOR_FG[1], // g
                focused ? 255 : COLOR_FG[2], // b
                1.0f, // a
                6,
                6,
                0,
                state
            );
        }
    }

    SPRITE_ARROW_DOWN->r = COLOR_FG[0];
    SPRITE_ARROW_DOWN->g = COLOR_FG[1];
    SPRITE_ARROW_DOWN->b = COLOR_FG[2];

    draw_positioned_sprite(
        SPRITE_ARROW_DOWN,
        self->x + self->width - SPRITE_ARROW_DOWN->width - 4,
        self->y + (h / 2) - (SPRITE_ARROW_DOWN->height / 2),
        base_z + 0.2f,
        SPRITE_ARROW_DOWN->width,
        SPRITE_ARROW_DOWN->height,
        state
    );
}

component_dropdown_list_option_T* init_component_dropdown_list_option(database_sprite_T* database_sprite, char* key, void* value, unsigned int text_limit)
{
    component_dropdown_list_option_T* option = calloc(1, sizeof(struct COMPONENT_DROPDOWN_LIST_OPTION_STRUCT));
    option->database_sprite = database_sprite;
    option->key = key;
    option->value = value;
    option->text_limit = text_limit;

    return option;
}

void component_dropdown_list_set_selected_option_by_string_value(component_dropdown_list_T* component_dropdown_list, const char* value)
{
    for (int i = 0; i < component_dropdown_list->options->size; i++)
    {
        component_dropdown_list_option_T* component_dropdown_list_option = (component_dropdown_list_option_T*) component_dropdown_list->options->items[i];

        if (component_dropdown_list_option->value == (void*)0)
        {
            if (value == (void*) 0)
            {
                component_dropdown_list->selected_index = i;
                break;
            }
            else
            {
                continue;
            }
        }

        if (strcmp((char*)component_dropdown_list_option->value, value) == 0)
        {
            component_dropdown_list->selected_index = i;
            break;
        }
    }
}

unsigned int component_dropdown_list_has_option_with_string_value(component_dropdown_list_T* component_dropdown_list, const char* value)
{
    for (int i = 0; i < component_dropdown_list->options->size; i++)
    {
        component_dropdown_list_option_T* component_dropdown_list_option = (component_dropdown_list_option_T*) component_dropdown_list->options->items[i];

        if (component_dropdown_list_option->value == (void*)0)
            continue;

        if (strcmp((char*)component_dropdown_list_option->value, value) == 0)
            return 1;
    }

    return 0;
}

void component_dropdown_list_update_option_sprite_by_string_value(
    component_dropdown_list_T* component_dropdown_list,
    const char* value,
    database_sprite_T* database_sprite
)
{
    for (int i = 0; i < component_dropdown_list->options->size; i++)
    {
        component_dropdown_list_option_T* component_dropdown_list_option = (component_dropdown_list_option_T*) component_dropdown_list->options->items[i];

        if (component_dropdown_list_option->database_sprite == (void*) 0)
            continue;

        if (component_dropdown_list_option->value == (void*) 0)
            continue;

        if (strcmp((char*)component_dropdown_list_option->value, value) == 0)
        {
            database_sprite_free(component_dropdown_list_option->database_sprite);
            component_dropdown_list_option->database_sprite = database_sprite;

            break;
        }
    }
}

void component_dropdown_list_update_option_key_by_string_value(
    component_dropdown_list_T* component_dropdown_list,
    const char* value,
    char* key        
)
{
    for (int i = 0; i < component_dropdown_list->options->size; i++)
    {
        component_dropdown_list_option_T* component_dropdown_list_option = (component_dropdown_list_option_T*) component_dropdown_list->options->items[i];

        if (component_dropdown_list_option->value == (void*)0)
            continue;

        if (strcmp((char*)component_dropdown_list_option->value, value) == 0)
        {
            free(component_dropdown_list_option->key);
            component_dropdown_list_option->key = key;

            break;
        }
    }
}

void component_dropdown_list_sync_from_table(component_dropdown_list_T* component_dropdown_list, database_T* database, const char* tablename, unsigned int key_column, int sprite_id_column)
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

        if (component_dropdown_list_has_option_with_string_value(component_dropdown_list, value))
        {
            if (sprite_id_column != -1)
            {
                printf("Update component_dropdown_list_option.\n");
                component_dropdown_list_update_option_sprite_by_string_value(
                    component_dropdown_list,
                    (const char*) value,
                    database_sprite        
                );
            }

            component_dropdown_list_update_option_key_by_string_value(
                component_dropdown_list,
                (const char*) value,
                key        
            );
        }
        else
        {
            dynamic_list_append(
                component_dropdown_list->options,
                init_component_dropdown_list_option(
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

void component_dropdown_list_reload_sprites(component_dropdown_list_T* component_dropdown_list)
{
    for (int i = 0; i < component_dropdown_list->options->size; i++)
    {
        component_dropdown_list_option_T* component_dropdown_list_option = (component_dropdown_list_option_T*) component_dropdown_list->options->items[i];

        if (component_dropdown_list_option->database_sprite != (void*) 0)
            database_sprite_reload_from_disk(component_dropdown_list_option->database_sprite);
    }
}

component_dropdown_list_option_T* component_dropdown_list_get_selected_option(component_dropdown_list_T* component_dropdown_list)
{
    for (int i = 0; i < component_dropdown_list->options->size; i++)
    {
        component_dropdown_list_option_T* component_dropdown_list_option = (component_dropdown_list_option_T*) component_dropdown_list->options->items[i];

        if (i == component_dropdown_list->selected_index)
        {
            return component_dropdown_list_option;
        }
    }

    return (void*) 0;
}
