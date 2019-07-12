#ifndef APOLLON_DATABASE_H
#define APOLLON_DATABASE_H
#include <coelum/dynamic_list.h>


typedef struct DATABASE_STRUCT
{
    dynamic_list_T* sprites;
} database_T;

database_T* init_database();
#endif
