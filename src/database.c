#include "include/database.h"


database_sprite_T* init_database_sprite(sprite_T* sprite, char* name)
{
    database_sprite_T* database_sprite = calloc(1, sizeof(struct DATABASE_SPRITE_STRUCT));
    database_sprite->sprite = sprite;
    database_sprite->name = name;

    return database_sprite;
}

database_T* init_database()
{
    database_T* database = calloc(1, sizeof(struct DATABASE_STRUCT));
    database->sprites = init_dynamic_list(sizeof(struct DATABASE_SPRITE_STRUCT*));

    return database;
}
