#ifndef APOLLON_COMPONENT_TEXTABLE
#define APOLLON_COMPONENT_TEXTABLE
#include "actor_component.h"
#include "focus_manager.h"
#include <sys/time.h>
#include <glad/glad.h>


typedef struct ACTOR_TEXTABLE
{
    actor_component_T base;
    char* value;
    int caret_position;
    unsigned int font_size;
    unsigned int font_spacing;
    unsigned int draw_caret;
    unsigned int supports_multiple_lines;
    int caret_x;
    int caret_y;
    int width;
    int height;
    int line_bar_width;
    int fg_r;
    int fg_g;
    int fg_b;
    int bg_r;
    int bg_g;
    int bg_b;
    struct timeval timer;
} component_textable_T;

component_textable_T* component_textable_constructor(
    focus_manager_T* focus_manager,
    component_textable_T* component_textable,
    float x,
    float y,
    int width,
    int height,
    unsigned int supports_multiple_lines
);

void component_textable_tick(actor_T* self);

void component_textable_draw(actor_T* self);

void component_textable_draw_line_numbers(component_textable_T* self);

void component_textable_draw_text_value(component_textable_T* self);

void component_textable_draw_caret(component_textable_T* self);

void component_textable_caret_blink(component_textable_T* self);

void component_textable_handle_keyboard_input(component_textable_T* self);

void component_textable_handle_character_input(component_textable_T* self);

int component_textable_calculate_scroll(component_textable_T* component_textable);

int component_textable_get_caret_line_number(component_textable_T* component_textable);

int component_textable_get_number_of_lines(component_textable_T* component_textable);
#endif
