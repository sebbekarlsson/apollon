#ifndef APOLLON_DROPDOWN_LIST_H
#define APOLLON_DROPDOWN_LIST_H
#include <coelum/actor.h>


typedef struct DROPDOWN_LIST_STRUCT
{
    actor_T base;
    dynamic_list_T* options;
    unsigned int focused;
    unsigned int option_index;
} dropdown_list_T;

dropdown_list_T* init_dropdown_list(float x, float y, float z);

void dropdown_list_tick(actor_T* self);

void dropdown_list_draw(actor_T* self);

typedef struct DROPDOWN_LIST_OPTION_STRUCT
{
    char* key;
    void* value;
} dropdown_list_option_T;

dropdown_list_option_T* init_dropdown_list_option(char* key, void* value);
#endif
