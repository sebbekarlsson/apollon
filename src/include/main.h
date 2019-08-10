#ifndef APOLLON_MAIN_H
#define APOLLON_MAIN_H
typedef struct MAIN_STATE_STRUCT
{
    char* scene_id;
    char* script_id;
    char* text_editor_value;
    unsigned int modal_is_active;
} main_state_T;

main_state_T* init_main_state();
#endif
