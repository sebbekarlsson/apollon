#ifndef APOLLON_TEXTAREA_H
#define APOLLON_TEXTAREA_H
#include "actor_focusable.h"
#include <sys/time.h>
#include <glad/glad.h>


typedef struct TEXTAREA_STRUCT
{
    actor_focusable_T base;
    char* value;
    char** lines;
    int nr_lines;
    int caret_x;
    int caret_y;
    unsigned int font_size;
    unsigned int font_spacing;
    unsigned int draw_caret;
    int width;
    int height;
    struct timeval timer;
} textarea_T;

textarea_T* init_textarea(float x, float y, float z, int width, int height);

void textarea_tick(actor_T* self);

void textarea_draw(actor_T* self);

void textarea_on_character_input(textarea_T* textarea, char c, char* text);

void textarea_shift_down(textarea_T* textarea);

void textarea_reposition_caret(textarea_T* textarea);

void textarea_insert_new_line(textarea_T* textarea, unsigned int position, unsigned int shift);
#endif
