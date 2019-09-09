#ifndef APOLLON_COMPONENT_DROPDOWN_LIST_H
#define APOLLON_COMPONENT_DROPDOWN_LIST_H
#include "actor_component.h"
#include "focus_manager.h"
#include <athena/database.h>


typedef struct COMPONENT_DROPDOWN_LIST_STRUCT
{
    actor_component_T base;
    dynamic_list_T* options;
    unsigned int focused;
    unsigned int option_index;
    unsigned int expanded;
    unsigned int selected_index;
    unsigned int deexpand_on_press;
    void (*press)(void* component_dropdown_list, void* option);
} component_dropdown_list_T;

component_dropdown_list_T* init_component_dropdown_list(focus_manager_T* focus_manager, float x, float y, float z, void (*press)(void* component_dropdown_list, void* option));

void component_dropdown_list_tick(actor_T* self);

void component_dropdown_list_draw(actor_T* self);

void component_dropdown_list_sync_from_table(component_dropdown_list_T* component_dropdown_list, database_T* database, const char* tablename, unsigned int key_column, int sprite_id_column);

void component_dropdown_list_reload_sprites(component_dropdown_list_T* component_dropdown_list);

void component_dropdown_list_set_selected_option_by_string_value(component_dropdown_list_T* component_dropdown_list, const char* value);

unsigned int component_dropdown_list_has_option_with_string_value(component_dropdown_list_T* component_dropdown_list, const char* value);

void component_dropdown_list_update_option_sprite_by_string_value(
    component_dropdown_list_T* component_dropdown_list,
    const char* value,
    database_sprite_T* database_sprite
);

void component_dropdown_list_update_option_key_by_string_value(
    component_dropdown_list_T* component_dropdown_list,
    const char* value,
    char* key        
);

typedef struct COMPONENT_DROPDOWN_LIST_OPTION_STRUCT
{
    database_sprite_T* database_sprite;
    char* key;
    void* value;
    unsigned int text_limit;
} component_dropdown_list_option_T;

component_dropdown_list_option_T* init_component_dropdown_list_option(database_sprite_T* database_sprite, char* key, void* value, unsigned int text_limit);

component_dropdown_list_option_T* component_dropdown_list_get_selected_option(component_dropdown_list_T* component_dropdown_list);
#endif
