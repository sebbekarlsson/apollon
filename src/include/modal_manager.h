#ifndef APOLLON_MODAL_MANAGER_H
#define APOLLON_MODAL_MANAGER_H
#include <coelum/dynamic_list.h>
#include "modal.h"


typedef struct MODAL_MANAGER_STRUCT
{
    dynamic_list_T* modals;
    float z;
} modal_manager_T;

modal_manager_T* init_modal_manager();

void modal_manager_draw(modal_manager_T* modal_manager);

void modal_manager_register_modal(modal_manager_T* modal_manager, modal_T* modal);

modal_T* modal_manager_show_modal(modal_manager_T* modal_manager, const char* title, const char* text);

void modal_manager_close_modal(modal_manager_T* modal_manager, modal_T* modal);

void modal_manager_close_all_modals(modal_manager_T* modal_manager);

void modal_manager_update(modal_manager_T* modal_manager);
#endif
