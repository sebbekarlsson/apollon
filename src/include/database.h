#ifndef APOLLON_DATABASE_H
#define APOLLON_DATABASE_H
#include <coelum/dynamic_list.h>
#include <coelum/sprite.h>
#include <sqlite3.h>


typedef struct DATABASE_SPRITE_STRUCT
{
    const unsigned char* id;
    const unsigned char* name;
    const unsigned char* filepath;
} database_sprite_T;

database_sprite_T* init_database_sprite(const unsigned char* id, const unsigned char* name, const unsigned char* filepath);

typedef struct DATABASE_STRUCT
{
    const char* filename;
    sqlite3* db;
} database_T;

database_T* init_database();

sqlite3_stmt* database_exec_sql(database_T* database, char* sql, unsigned int do_error_checking);

char* database_insert_sprite(database_T* database, const char* name, sprite_T* sprite);

dynamic_list_T* database_get_all_sprites(database_T* database);
#endif
