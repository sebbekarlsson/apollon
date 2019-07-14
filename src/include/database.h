#ifndef APOLLON_DATABASE_H
#define APOLLON_DATABASE_H
#include <coelum/dynamic_list.h>
#include <coelum/sprite.h>
#include <coelum/scene.h>

static const char* DATABASE_FILENAME = "application.db";

typedef struct DATABASE_SPRITE_STRUCT
{
    sprite_T* sprite;
    char* name;
} database_sprite_T;

database_sprite_T* init_database_sprite(sprite_T* sprite, char* name);

typedef struct DATABASE_ACTOR_DEFINITION_STRUCT
{
    database_sprite_T* database_sprite;
    char* name;
    char* tick_script;
    char* draw_script;
} database_actor_definition_T;

database_actor_definition_T* init_database_actor_definition(database_sprite_T* database_actor_definition, char* name, char* tick_script, char* draw_script);

typedef struct DATABASE_STRUCT
{
    dynamic_list_T* sprites;
    dynamic_list_T* actor_definitions;
    dynamic_list_T* scenes;
} database_T;

database_T* init_database();

void database_serialize(database_T* database);

void database_deserialize(database_T* database, const char* filename);
#endif
