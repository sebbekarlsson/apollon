#ifndef APOLLON_DROPDOWN_LIST_H
#define APOLLON_DROPDOWN_LIST_H
#include "actor_focusable.h"
#include "database.h"


typedef struct DROPDOWN_LIST_STRUCT
{
    actor_focusable_T base;
    dynamic_list_T* options;
    unsigned int focused;
    unsigned int option_index;
    unsigned int expanded;
    unsigned int selected_index;
    unsigned int width;
    void (*press)(void* dropdown_list, void* option);
} dropdown_list_T;

dropdown_list_T* init_dropdown_list(float x, float y, float z, void (*press)(void* dropdown_list, void* option));

void dropdown_list_tick(actor_T* self);

void dropdown_list_draw(actor_T* self);

void dropdown_list_sync_from_table(dropdown_list_T* dropdown_list, database_T* database, const char* tablename, unsigned int key_column, int sprite_column);

unsigned int dropdown_list_has_option_with_string_value(dropdown_list_T* dropdown_list, const char* value);

typedef struct DROPDOWN_LIST_OPTION_STRUCT
{
    sprite_T* sprite;
    char* key;
    void* value;
    unsigned int text_limit;
} dropdown_list_option_T;

dropdown_list_option_T* init_dropdown_list_option(sprite_T* sprite, char* key, void* value, unsigned int text_limit);
#endif
