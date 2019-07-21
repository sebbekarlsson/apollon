#ifndef APOLLON_DATABASE_H
#define APOLLON_DATABASE_H
#include <coelum/dynamic_list.h>
#include <coelum/sprite.h>
#include <sqlite3.h>


typedef struct DATABASE_SPRITE_STRUCT
{
    char* id;
    char* name;
    char* filepath;
    sprite_T* sprite;
} database_sprite_T;

database_sprite_T* init_database_sprite(char* id, char* name, char* filepath, sprite_T* sprite);

void database_sprite_free(database_sprite_T* database_sprite);

void database_sprite_reload_from_disk(database_sprite_T* database_sprite);

typedef struct DATABASE_ACTOR_DEFINITION_STRUCT
{
    char* id;
    char* name;
    char* sprite_id;
    char* tick_script;
    char* draw_script;
} database_actor_definition_T;

database_actor_definition_T* init_database_actor_definition(
    char* id,
    char* name,
    char* sprite_id,
    char* tick_script,
    char* draw_script    
);

typedef struct DATABASE_STRUCT
{
    const char* filename;
    sqlite3* db;
} database_T;

database_T* init_database();

sqlite3_stmt* database_exec_sql(database_T* database, char* sql, unsigned int do_error_checking);

char* database_insert_sprite(database_T* database, const char* name, sprite_T* sprite);

void database_update_sprite_name_by_id(database_T* database, const char* id, const char* name);

database_sprite_T* database_get_sprite_by_id(database_T* database, const char* id);

char* database_insert_actor_definition(
    database_T* database,
    const char* name,
    const char* sprite_id,
    const char* tick_script,
    const char* draw_script
);

database_actor_definition_T* database_get_actor_definition_by_id(database_T* database, const char* id);

void database_update_actor_definition_by_id(
    database_T* database,
    const char* id,
    const char* name,
    const char* sprite_id,
    const char* tick_script,
    const char* draw_script
);
#endif
