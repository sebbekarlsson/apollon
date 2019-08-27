#ifndef APOLLON_ACTOR_TEXTABLE
#define APOLLON_ACTOR_TEXTABLE
#include "actor_focusable.h"
#include <sys/time.h>
#include <glad/glad.h>


typedef struct ACTOR_TEXTABLE
{
    actor_focusable_T base;
    char* value;
    int caret_position;
    unsigned int font_size;
    unsigned int font_spacing;
    unsigned int draw_caret;
    unsigned int supports_multiple_lines;
    int caret_x;
    int caret_y;
    int fg_r;
    int fg_g;
    int fg_b;
    struct timeval timer;
} actor_textable_T;

actor_textable_T* actor_textable_constructor(
    actor_textable_T* actor_textable,
    float x,
    float y,
    int width,
    int height,
    unsigned int supports_multiple_lines
);

void actor_textable_tick(actor_T* self);

void actor_textable_draw(actor_T* self);

void actor_textable_draw_text_value(actor_textable_T* self);

void actor_textable_draw_caret(actor_textable_T* self);

void actor_textable_caret_blink(actor_textable_T* self);

void actor_textable_handle_keyboard_input(actor_textable_T* self);

void actor_textable_handle_character_input(actor_textable_T* self);

int actor_textable_calculate_scroll(actor_textable_T* actor_textable);

int actor_textable_get_caret_line_number(actor_textable_T* actor_textable);

int actor_textable_get_number_of_lines(actor_textable_T* actor_textable);
#endif
