#ifndef APOLLON_DATABASE_H
#define APOLLON_DATABASE_H
#include <coelum/dynamic_list.h>
#include <coelum/sprite.h>


typedef struct DATABASE_SPRITE_STRUCT
{
    sprite_T* sprite;
    char* name;
} database_sprite_T;

database_sprite_T* init_database_sprite(sprite_T* sprite, char* name);

typedef struct DATABASE_STRUCT
{
    dynamic_list_T* sprites;
} database_T;

database_T* init_database();
#endif
