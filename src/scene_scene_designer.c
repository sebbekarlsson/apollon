#include "include/main.h"
#include "include/scene_scene_designer.h"
#include "include/etc.h"
#include <athena/database.h>
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
extern const float COLOR_FG[3];
extern sprite_T* SPRITE_BROKEN;
extern GLFWwindow* window;


void _free_database_actor_instance(void* item)
{
    database_actor_instance_T* database_actor_instance = (database_actor_instance_T*) item;
    database_actor_instance_free(database_actor_instance);
}

void scene_designer_load(scene_T* self)
{
    scene_scene_designer_T* s_scene_designer = (scene_scene_designer_T*) self;

    if (s_scene_designer->database_scene != (void*) 0)
    {
        database_scene_free(s_scene_designer->database_scene);
        s_scene_designer->database_scene = (void*) 0;
    }

    if (MAIN_STATE->scene_id != (void*) 0)
        s_scene_designer->database_scene = database_get_scene_by_id(DATABASE, MAIN_STATE->scene_id);

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

    component_dropdown_list_sync_from_table(
        s_scene_designer->component_dropdown_list,
        DATABASE,
        "actor_definitions",
        1,
        5
    );
    component_dropdown_list_reload_sprites(s_scene_designer->component_dropdown_list);
}

void scene_designer_component_dropdown_press(void* component_dropdown_list, void* option)
{
    printf("press\n");
    scene_T* scene = get_current_scene();
    scene_scene_designer_T* s_scene_designer = (scene_scene_designer_T*) scene;
    actor_cursor_T* actor_cursor = (actor_cursor_T*) s_scene_designer->actor_cursor;

    component_dropdown_list_T* component_dropdown = (component_dropdown_list_T*) component_dropdown_list;
    actor_component_T* component_dropdown_list_component = (actor_component_T*) component_dropdown;
    component_dropdown_list_option_T* component_dropdown_list_option = (component_dropdown_list_option_T*) option;

    database_insert_actor_instance(
        DATABASE,
        (const char*) component_dropdown_list_option->value,
        (const char*) MAIN_STATE->scene_id,
        (const float) s_scene_designer->clicked_x,
        (const float) s_scene_designer->clicked_y,
        (const float) ((actor_T*)actor_cursor)->z
    );

    scene_scene_designer_refresh_state(s_scene_designer);
}

void scene_scene_designer_update_coords_text(scene_scene_designer_T* s_scene_designer)
{
    actor_T* actor = (actor_T*) s_scene_designer->actor_cursor;

    const char* template = "x: %1.2f y: %1.2f";
    free(s_scene_designer->coords_text);
    s_scene_designer->coords_text = calloc(
        (strlen(template) + 1) + 128,
        sizeof(char)
    );
    sprintf(
        s_scene_designer->coords_text,
        template,
        actor->x,
        actor->y
    );
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
    s_scene_designer->database_scene = (void*) 0;
    s_scene_designer->coords_text = calloc(1, sizeof(char));
    s_scene_designer->coords_text[0] = '\0';
    s_scene_designer->focus_manager = init_focus_manager();
    s_scene_designer->clicked_x = 0;
    s_scene_designer->clicked_y = 0;


    // this one is starts as focused
    s_scene_designer->component_dropdown_list = init_component_dropdown_list(
            s_scene_designer->focus_manager,
            0.0f, 0.0f, 0.0f, scene_designer_component_dropdown_press);
    ((actor_component_T*)s_scene_designer->component_dropdown_list)->visible = 0;
    ((actor_T*)s_scene_designer->component_dropdown_list)->z = 1;
    
    dynamic_list_append(s_scene_designer->focus_manager->components, s_scene_designer->component_dropdown_list); 
    dynamic_list_append(state->actors, s_scene_designer->component_dropdown_list);

    s_scene_designer->actor_cursor = init_actor_cursor(
        RES_WIDTH / 2,
        RES_HEIGHT / 2,
        0.0f
    );

    s_scene_designer->database_actor_instances = init_dynamic_list(sizeof(struct DATABASE_ACTOR_INSTANCE_STRUCT*));

    scene_scene_designer_update_coords_text(s_scene_designer);

    dynamic_list_append(state->actors, s_scene_designer->actor_cursor);

    return s_scene_designer;
}

void scene_scene_designer_tick(scene_T* self)
{
    go_back_on_escape();

    scene_scene_designer_T* s_scene_designer = (scene_scene_designer_T*) self;
    actor_cursor_T* actor_cursor = (actor_cursor_T*) s_scene_designer->actor_cursor;

    focus_manager_tick(s_scene_designer->focus_manager);

    ((actor_T*)actor_cursor)->x = ((int)MOUSE_STATE->x / 16) * 16;
    ((actor_T*)actor_cursor)->y = ((int)MOUSE_STATE->y / 16) * 16;

    unsigned int click = 0;
    static int oldState = GLFW_RELEASE;
    int newState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (newState == GLFW_RELEASE && oldState == GLFW_PRESS)
    {
       click = 1;
    }
    oldState = newState;

    if (click)
    { // expand dropdown and make it visible
        s_scene_designer->clicked_x = ((actor_T*)actor_cursor)->x;
        s_scene_designer->clicked_y = ((actor_T*)actor_cursor)->y;
        ((actor_T*)s_scene_designer->component_dropdown_list)->x = ((actor_T*)actor_cursor)->x;
        ((actor_T*)s_scene_designer->component_dropdown_list)->y = ((actor_T*)actor_cursor)->y;
        actor_component_T* component_dropdown_list_component = (actor_component_T*) s_scene_designer->component_dropdown_list;

        component_dropdown_list_component->focused = 1;
        s_scene_designer->component_dropdown_list->expanded = 1;
        ((actor_component_T*)s_scene_designer->component_dropdown_list)->visible = 1;
    }

    if (KEYBOARD_STATE->keys[GLFW_KEY_DELETE] && !KEYBOARD_STATE->key_locks[GLFW_KEY_DELETE])
    {
        float cx = ((actor_T*)actor_cursor)->x;
        float cy = ((actor_T*)actor_cursor)->y;
        
        for (int i = 0; i < s_scene_designer->database_actor_instances->size; i++)
        {
            database_actor_instance_T* dai = (database_actor_instance_T*) s_scene_designer->database_actor_instances->items[i];

            if (cx + 16 >= dai->x && cx < dai->x + 16)
            {
                if (cy + 16 >= dai->y && cy < dai->y + 16)
                {
                    database_delete_actor_instance_by_id(DATABASE, dai->id);
                    dynamic_list_remove(s_scene_designer->database_actor_instances, dai, _free_database_actor_instance);
                    break;
                }
            }
        }

        KEYBOARD_STATE->key_locks[GLFW_KEY_DELETE] = 1;
    }
}

void scene_scene_designer_draw(scene_T* self)
{
    scene_scene_designer_T* s_scene_designer = (scene_scene_designer_T*) self;
    state_T* state = (state_T*) self;

    if (MAIN_STATE->scene_id != (void*) 0)
    {
        if (s_scene_designer->database_scene != (void*) 0)
        {
            draw_text(
                s_scene_designer->database_scene->name,
                6 + 4,
                16 - 6,
                0,
                COLOR_FG[0], // r
                COLOR_FG[1], // g
                COLOR_FG[2], // b
                1.0f, // a
                6,
                6,
                0,
                state
            );
        }
    }

    draw_text(
        s_scene_designer->coords_text,
        (strlen(s_scene_designer->database_scene->name) * (6 + 6)) + 6 + 4 + 6 + 4,
        16 - 6,
        0,
        COLOR_FG[0], // r
        COLOR_FG[1], // g
        COLOR_FG[2], // b
        1.0f, // a
        6,
        6,
        0,
        state
    );

    for (int i = 0; i < s_scene_designer->database_actor_instances->size; i++)
    {
        database_actor_instance_T* database_actor_instance = (database_actor_instance_T*) s_scene_designer->database_actor_instances->items[i];

        database_actor_definition_T* def = database_actor_instance->database_actor_definition;
        sprite_T* sprite_to_be_drawn = (void*) 0;

        if (def->database_sprite != (void*) 0)
        {
            if (def->database_sprite->sprite != (void*) 0)
            {
                sprite_to_be_drawn = def->database_sprite->sprite;
            }
        }

        if (sprite_to_be_drawn == (void*) 0)
        {
            sprite_to_be_drawn = SPRITE_BROKEN;
        }

        draw_positioned_sprite(
            sprite_to_be_drawn,
            database_actor_instance->x,
            database_actor_instance->y,
            database_actor_instance->z,
            16,
            16,
            state
        );
    }
}

void scene_scene_designer_empty_database_actor_instances(scene_scene_designer_T* s_scene_designer)
{
    if (s_scene_designer->database_actor_instances->size > 0)
    {
        for (int i = s_scene_designer->database_actor_instances->size; i > 0; i--)
        {
            dynamic_list_remove(
                s_scene_designer->database_actor_instances,
                s_scene_designer->database_actor_instances->items[i-1],
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
