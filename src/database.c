#include "include/database.h"
#include <sqlite3.h>


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
    database->sprites = init_dynamic_list(sizeof(struct DATABASE_SPRITE_STRUCT*));
    database->actor_definitions = init_dynamic_list(sizeof(struct DATABASE_ACTOR_DEFINITION_STRUCT*));
    database->scenes = init_dynamic_list(sizeof(struct SCENE_STRUCT*)); 

    sqlite3 *db;
    char *err_msg = 0;
    
    int rc = sqlite3_open(DATABASE_FILENAME, &db);
    
    if (rc != SQLITE_OK)
    {
        
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return database;
    }
    
    char *sql = "CREATE TABLE IF NOT EXISTS actor_definitions(id INT, name TEXT, tick_script TEXT, draw_script TEXT, sprite_id INT);"
                "CREATE TABLE IF NOT EXISTS actor_instances(id INT, actor_definition_id INT, x FLOAT, y FLOAT, z FLOAT, scene_id INT);"
                "CREATE TABLE IF NOT EXISTS sprites(id INT, name TEXT, filepath TEXT)"; 

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK )
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
}

void database_deserialize(database_T* database, const char* filename)
{
}
