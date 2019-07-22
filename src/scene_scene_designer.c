#include "include/main.h"
#include "include/scene_scene_designer.h"
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
extern main_state_T* MAIN_STATE;


void scene_designer_load(scene_T* self)
{
    scene_scene_designer_T* s_scene_designer = (scene_scene_designer_T*) self;
    scene_scene_designer_empty_database_actor_instances(s_scene_designer);
    scene_scene_designer_refresh_state(s_scene_designer);
}

void scene_scene_designer_refresh_state(scene_scene_designer_T* s_scene_designer)
{
    printf("Refreshing state...\n");

    s_scene_designer->scene_count = database_count_scenes(DATABASE);
    printf("Number of scenes: %d\n", s_scene_designer->scene_count);

    if (MAIN_STATE->scene_id != (void*)0)
        scene_scene_designer_sync_database_actor_instances(s_scene_designer);

    dropdown_list_sync_from_table(
        s_scene_designer->dropdown_list,
        DATABASE,
        "actor_definitions",
        1,
        4
    );
    dropdown_list_reload_sprites(s_scene_designer->dropdown_list);
}

void scene_designer_dropdown_press(void* dropdown_list, void* option)
{
    scene_T* scene = get_current_scene();
    scene_scene_designer_T* s_scene_designer = (scene_scene_designer_T*) scene;
    actor_cursor_T* actor_cursor = (actor_cursor_T*) s_scene_designer->actor_cursor;

    dropdown_list_T* dropdown = (dropdown_list_T*) dropdown_list;
    actor_focusable_T* dropdown_list_focusable = (actor_focusable_T*) dropdown;
    dropdown_list_option_T* dropdown_list_option = (dropdown_list_option_T*) option;

    database_insert_actor_instance(
        DATABASE,
        (const char*) dropdown_list_option->value,
        (const char*) MAIN_STATE->scene_id,
        (const float) ((actor_T*)actor_cursor)->x,
        (const float) ((actor_T*)actor_cursor)->y,
        (const float) ((actor_T*)actor_cursor)->z
    );

    // close dropdown
    dropdown_list_focusable->focused = 0;
    ((actor_focusable_T*)s_scene_designer->dropdown_list)->visible = 0;
    
    scene_scene_designer_refresh_state(s_scene_designer);
}

scene_scene_designer_T* init_scene_scene_designer()
{
    scene_scene_designer_T* s_scene_designer = calloc(1, sizeof(struct SCENE_SCENE_DESIGNER_STRUCT));
    scene_T* s = (scene_T*) s_scene_designer;
    state_T* state = (state_T*) s; 

    scene_constructor(s, scene_scene_designer_tick, scene_scene_designer_draw, 2);

    s->load = scene_designer_load;

    s->type_name = "scene_designer";
    s->bg_r = 255;
    s->bg_g = 255;
    s->bg_b = 255;

    s_scene_designer->scene_index = 0;
    s_scene_designer->scene_count = 0;

    // this one is starts as focused
    s_scene_designer->dropdown_list = init_dropdown_list(0.0f, 0.0f, 0.0f, scene_designer_dropdown_press);
    ((actor_focusable_T*)s_scene_designer->dropdown_list)->visible = 0;
    ((actor_T*)s_scene_designer->dropdown_list)->z = 1;
    
    dynamic_list_append(state->actors, s_scene_designer->dropdown_list);

    s_scene_designer->actor_cursor = init_actor_cursor(
        WINDOW_WIDTH / 2,
        WINDOW_HEIGHT / 2,
        0.0f
    );

    s_scene_designer->database_actor_instances = init_dynamic_list(sizeof(struct DATABASE_ACTOR_INSTANCE_STRUCT*));

    dynamic_list_append(state->actors, s_scene_designer->actor_cursor);

    return s_scene_designer;
}

void scene_scene_designer_tick(scene_T* self)
{
    go_back_on_escape();

    scene_scene_designer_T* s_scene_designer = (scene_scene_designer_T*) self;
    actor_cursor_T* actor_cursor = (actor_cursor_T*) s_scene_designer->actor_cursor;

    if (KEYBOARD_STATE->keys[GLFW_KEY_UP] && !KEYBOARD_STATE->key_locks[GLFW_KEY_UP])
    {
        ((actor_T*)actor_cursor)->y -= 16;
        KEYBOARD_STATE->key_locks[GLFW_KEY_UP] = 1;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_DOWN] && !KEYBOARD_STATE->key_locks[GLFW_KEY_DOWN])
    {
        ((actor_T*)actor_cursor)->y += 16;
        KEYBOARD_STATE->key_locks[GLFW_KEY_DOWN] = 1;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_LEFT] && !KEYBOARD_STATE->key_locks[GLFW_KEY_LEFT])
    {
        ((actor_T*)actor_cursor)->x -= 16;
        KEYBOARD_STATE->key_locks[GLFW_KEY_LEFT] = 1;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_RIGHT] && !KEYBOARD_STATE->key_locks[GLFW_KEY_RIGHT])
    {
        ((actor_T*)actor_cursor)->x += 16;
        KEYBOARD_STATE->key_locks[GLFW_KEY_RIGHT] = 1;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_I] && !KEYBOARD_STATE->key_locks[GLFW_KEY_I])
    {
        ((actor_T*)s_scene_designer->dropdown_list)->x = ((actor_T*)actor_cursor)->x;
        ((actor_T*)s_scene_designer->dropdown_list)->y = ((actor_T*)actor_cursor)->y;
        actor_focusable_T* dropdown_list_focusable = (actor_focusable_T*) s_scene_designer->dropdown_list;

        if (!dropdown_list_focusable->focused)
        {
            dropdown_list_focusable->focused = 1;
            ((actor_focusable_T*)s_scene_designer->dropdown_list)->visible = 1;
            printf("Please show dropdown\n");
        }
        else
        {
            dropdown_list_focusable->focused = 0;
            ((actor_focusable_T*)s_scene_designer->dropdown_list)->visible = 0;
            printf("Please dont show dropdown\n");
        }

        KEYBOARD_STATE->key_locks[GLFW_KEY_I] = 1;
    }
}

void scene_scene_designer_draw(scene_T* self)
{
    scene_scene_designer_T* s_scene_designer = (scene_scene_designer_T*) self;
    state_T* state = (state_T*) self;

    if (MAIN_STATE->scene_id != (void*) 0)
    {
        draw_text(
            MAIN_STATE->scene_id,
            16 + 6,
            16 - 6,
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

    for (int i = 0; i < s_scene_designer->database_actor_instances->size; i++)
    {
        database_actor_instance_T* database_actor_instance = (database_actor_instance_T*) s_scene_designer->database_actor_instances->items[i];
        draw_positioned_sprite(
            database_actor_instance->database_actor_definition->database_sprite->sprite,
            database_actor_instance->x,
            database_actor_instance->y,
            database_actor_instance->z,
            16,
            16,
            state
        );
    }
}

void _free_database_actor_instance(void* item)
{
    database_actor_instance_T* database_actor_instance = (database_actor_instance_T*) item;
    database_actor_instance_free(database_actor_instance);
}

void scene_scene_designer_empty_database_actor_instances(scene_scene_designer_T* s_scene_designer)
{
    if (s_scene_designer->database_actor_instances->size > 0)
    {
        for (int i = s_scene_designer->database_actor_instances->size; i > 0; i--)
        {
            dynamic_list_remove(
                s_scene_designer->database_actor_instances,
                s_scene_designer->database_actor_instances->items[i],
                _free_database_actor_instance
            );
        }
    }
}

void scene_scene_designer_sync_database_actor_instances(scene_scene_designer_T* s_scene_designer)
{
    char* sql_template = "SELECT * FROM actor_instances WHERE scene_id=\"%s\"";
    char* sql = calloc(strlen(sql_template) + strlen(MAIN_STATE->scene_id) + 1, sizeof(char));
    sprintf(sql, sql_template, MAIN_STATE->scene_id);

    sqlite3_stmt* stmt = database_exec_sql(DATABASE, sql, 0);

	while (sqlite3_step(stmt) != SQLITE_DONE) {
        const unsigned char* id = sqlite3_column_text(stmt, 0);
        const unsigned char* actor_definition_id = sqlite3_column_text(stmt, 1);
        const float x = sqlite3_column_double(stmt, 2);
        const float y = sqlite3_column_double(stmt, 3);
        const float z = sqlite3_column_double(stmt, 4);
        const unsigned char* scene_id = sqlite3_column_text(stmt, 5);

        if (!scene_scene_designer_has_database_actor_instance_by_id(s_scene_designer, id))
        {
            printf("Adding actor instance for drawing\n");

            char* id_new = calloc(strlen(id) + 1, sizeof(char));
            strcpy(id_new, id);

            char* actor_definition_id_new = calloc(strlen(actor_definition_id) + 1, sizeof(char));
            strcpy(actor_definition_id_new, actor_definition_id);

            char* scene_id_new = calloc(strlen(scene_id) + 1, sizeof(char));
            strcpy(scene_id_new, scene_id);

            database_actor_instance_T* database_actor_instance = init_database_actor_instance(
                id_new,
                actor_definition_id_new,
                scene_id_new,
                x,
                y,
                z,
                database_get_actor_definition_by_id(DATABASE, actor_definition_id)
            );

            dynamic_list_append(s_scene_designer->database_actor_instances, database_actor_instance);
        }
	}

	sqlite3_finalize(stmt);

	sqlite3_close(DATABASE->db);
}

unsigned int scene_scene_designer_has_database_actor_instance_by_id(scene_scene_designer_T* s_scene_designer, const char* id)
{
    for (int i = 0; i < s_scene_designer->database_actor_instances->size; i++)
    {
        database_actor_instance_T* database_actor_instance = (database_actor_instance_T*) s_scene_designer->database_actor_instances->items[i];

        if (strcmp(database_actor_instance->id, id) == 0)
            return 1;
    }

    return 0;
}
