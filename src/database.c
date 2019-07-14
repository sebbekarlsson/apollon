#include "include/database.h"


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

    return database;
}
