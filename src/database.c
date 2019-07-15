#include "include/database.h"
#include <sqlite3.h>
#include <string.h>


database_sprite_T* init_database_sprite(sprite_T* sprite, char* name)
{
    database_sprite_T* database_sprite = calloc(1, sizeof(struct DATABASE_SPRITE_STRUCT));
    database_sprite->sprite = sprite;
    database_sprite->name = name;

    return database_sprite;
}

database_actor_definition_T* init_database_actor_definition(database_sprite_T* database_sprite, char* name, char* tick_script, char* draw_script)
{
    database_actor_definition_T* database_actor_definition = calloc(1, sizeof(struct DATABASE_ACTOR_DEFINITION_STRUCT));
    database_actor_definition->database_sprite = database_sprite;
    database_actor_definition->name = name;
    database_actor_definition->tick_script = tick_script;
    database_actor_definition->draw_script = draw_script;

    return database_actor_definition;
}

database_T* init_database()
{
    database_T* database = calloc(1, sizeof(struct DATABASE_STRUCT));
    database->filename = "application.db";
    database->sprites = init_dynamic_list(sizeof(struct DATABASE_SPRITE_STRUCT*));
    database->actor_definitions = init_dynamic_list(sizeof(struct DATABASE_ACTOR_DEFINITION_STRUCT*));
    database->scenes = init_dynamic_list(sizeof(struct SCENE_STRUCT*)); 

    sqlite3 *db;
    char *err_msg = 0;
    
    int rc = sqlite3_open(database->filename, &db);
    
    if (rc != SQLITE_OK)
    {
        
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return database;
    }
    
    char *sql = "CREATE TABLE IF NOT EXISTS actor_definitions(id INT, name TEXT, tick_script TEXT, draw_script TEXT, sprite_id INT);"
                "CREATE TABLE IF NOT EXISTS actor_instances(id INT, actor_definition_id INT, x FLOAT, y FLOAT, z FLOAT, scene_id INT);"
                "CREATE TABLE IF NOT EXISTS sprites(id INT, name TEXT, filepath TEXT);"
                "CREATE TABLE IF NOT EXISTS scenes(id INT, name TEXT, bg_r INT, bg_g INT, bg_b INT)"; 

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK)
    {
        
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        
        return database;
    } 
    
    sqlite3_close(db);

    return database;
}

void database_serialize(database_T* database)
{
    sqlite3 *db;
    char *err_msg = 0;
    
    int rc = sqlite3_open(database->filename, &db);
    
    if (rc != SQLITE_OK)
    {
        
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return;
    }

    char* sprites_sql = database_get_sprites_sql(database);
    char* actor_definitions_sql = database_get_actor_definitions_sql(database);
    char* actor_instances_sql = database_get_actor_instances_sql(database);
    char* scenes_sql = database_get_scenes_sql(database);

    char* sql = calloc(
        strlen(sprites_sql) +
        strlen(actor_definitions_sql) + 
        strlen(actor_instances_sql) +
        strlen(scenes_sql) + 1,
        sizeof(char)
    );

    strcat(sql, sprites_sql);
    strcat(sql, actor_definitions_sql);
    strcat(sql, actor_instances_sql);
    strcat(sql, scenes_sql);

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK)
    {
        
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        
        return;
    } 
    
    sqlite3_close(db);
}

void database_deserialize(database_T* database, const char* filename)
{
}

char* database_get_sprites_sql(database_T* database)
{
    char* sql = calloc(1, sizeof(char));
    sql[0] = '\0';

    for (int i = 0; i < database->sprites->size; i++)
    {
        database_sprite_T* database_sprite = (database_sprite_T*) database->sprites->items[i];
        char* item_sql_template = "INSERT INTO sprites VALUES(%d, \"%s\", \"abc123\");\n";
        char* item_sql = calloc(256, sizeof(char));

        sprintf(item_sql, item_sql_template, i, database_sprite->name);

        sql = realloc(sql, (strlen(sql) + strlen(item_sql)) * sizeof(char));
        strcat(sql, item_sql);

        free(item_sql);
    }

    printf("SPRITES_SQL: %s", sql);

    return sql;
}

char* database_get_actor_definitions_sql(database_T* database)
{
    char* sql = calloc(1, sizeof(char));
    sql[0] = '\0';

    for (int i = 0; i < database->actor_definitions->size; i++)
    {
        database_actor_definition_T* database_actor_definition = (database_actor_definition_T*) database->actor_definitions->items[i];
        char* item_sql_template = "INSERT INTO actor_definitions VALUES(%d, \"%s\", \"%s\", \"%s\", %d)\n";
        char* item_sql = calloc(256, sizeof(char));

        sprintf(item_sql, item_sql_template, i, database_actor_definition->name, database_actor_definition->tick_script, database_actor_definition->draw_script, 0);

        sql = realloc(sql, (strlen(sql) + strlen(item_sql)) * sizeof(char));
        strcat(sql, item_sql);

        free(item_sql);
    }

    printf("ACTOR_DEFINITIONS_SQL: %s", sql);
    return sql;
}

char* database_get_scenes_sql(database_T* database)
{
    return 0;
}

char* database_get_actor_instances_sql(database_T* database)
{
    return 0;
}
