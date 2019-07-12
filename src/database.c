#include "include/database.h"


database_T* init_database()
{
    database_T* database = calloc(1, sizeof(struct DATABASE_STRUCT));
    database->sprites = init_dynamic_list(sizeof(struct SPRITE_STRUCT*));

    return database;
}
