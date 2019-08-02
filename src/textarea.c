#include "include/textarea.h"
#include <coelum/current.h>
#include <coelum/draw_utils.h>
#include <coelum/constants.h>
#include <coelum/input.h>
#include <string.h>


extern keyboard_state_T* KEYBOARD_STATE;


char* str_get_left(char* str, int position)
{
    char* left = calloc(1, sizeof(char));
    left[0] = '\0';
    size_t left_size = 1;

    for (int i = 0; i < position; i++)
    {
        left_size += 1;
        left = realloc(left, (left_size + 1) * sizeof(char));
        left[left_size-2] = str[i];
        left[left_size-1] = '\0';
    }

    return left;
}

char* str_get_right(char* str, int position)
{
    char* right = calloc(1, sizeof(char));
    right[0] = '\0';
    size_t right_size = 1;

    for (int i = position + 1; i < strlen(str); i++)
    {
        right_size += 1;
        right = realloc(right, (right_size + 1) * sizeof(char));
        right[right_size-2] = str[i];
        right[right_size-1] = '\0';
    }

    return right;
}

void str_erase(char** str, int position)
{
    char* source = *str;

    char* left = str_get_left(source, position);
    char* right = str_get_right(source, position); 

    char* new_str = calloc(strlen(left) + strlen(right) + 1, sizeof(char));
    new_str[0] = '\0';
    strcat(new_str, left);
    strcat(new_str, right);

    free(*str);
    *str = new_str;

    free(left);
    free(right);
}

static char* substring(char *string, int position, int length)
{
   char *pointer;
   int c;
 
   pointer = malloc(length+1);
   
   if(pointer == (void*) 0)
       exit(EXIT_FAILURE);
 
   for(c = 0 ; c < length ; c++)
      *(pointer+c) = *((string+position-1)+c);      
       
   *(pointer+c) = '\0';
 
   return pointer;
}

static void insert_substring(char *a, char *b, int position)
{
   char *f, *e;
   int length;
   
   length = strlen(a);
   
   f = substring(a, 1, position - 1 );      
   e = substring(a, position, length-position+1);
 
   strcpy(a, "");
   strcat(a, f);
   free(f);
   strcat(a, b);
   strcat(a, e);
   free(e);
}

textarea_T* init_textarea(float x, float y, float z, int width, int height)
{
    textarea_T* textarea = calloc(1, sizeof(struct TEXTAREA_STRUCT));
    actor_T* actor = (actor_T*) textarea;
    actor_constructor(actor, x, y, z, textarea_tick, textarea_draw, "textarea");
    actor_focusable_T* actor_focusable = (actor_focusable_T*) textarea;
    actor_focusable_constructor(actor_focusable);

    textarea->value = calloc(1, sizeof(char));
    textarea->value[0] = '\0';
    textarea->font_size = 8;
    textarea->font_spacing = textarea->font_size + 4;
    textarea->width = width;
    textarea->height = height;
    textarea->lines = calloc(1, sizeof(char*));
    textarea->lines[0] = calloc(1, sizeof(char));
    textarea->lines[0][0] = '\0';
    textarea->nr_lines = 1;

    textarea->caret_x = 0;
    textarea->caret_y = 0;
    textarea->draw_caret = 1;

    gettimeofday(&textarea->timer, 0);

    return textarea;
}

void textarea_tick(actor_T* self)
{
    textarea_T* textarea = (textarea_T*) self;
    actor_focusable_T* actor_focusable = (actor_focusable_T*) textarea;

    if (!actor_focusable->focused)
        return;

    if (
        (KEYBOARD_STATE->keys[GLFW_KEY_BACKSPACE] && !KEYBOARD_STATE->key_locks[GLFW_KEY_BACKSPACE]) ||
        (KEYBOARD_STATE->keys[GLFW_KEY_LEFT] && !KEYBOARD_STATE->key_locks[GLFW_KEY_LEFT])
    )
    {
        if (textarea->caret_x > 0)
            textarea->caret_x -= 1;

        if (KEYBOARD_STATE->keys[GLFW_KEY_BACKSPACE])
        {
            str_erase(&textarea->lines[textarea->caret_y], textarea->caret_x);
            
            if (textarea->caret_x == 0 && textarea->caret_y > 0)
            {
                textarea->caret_y -= 1;
                textarea->caret_x = strlen(textarea->lines[textarea->caret_y]);
            }

            KEYBOARD_STATE->key_locks[GLFW_KEY_BACKSPACE] = 1;
        }
        else
        {
            KEYBOARD_STATE->key_locks[GLFW_KEY_LEFT] = 1;
        }
    }
    else
    if (KEYBOARD_STATE->keys[GLFW_KEY_RIGHT] && !KEYBOARD_STATE->key_locks[GLFW_KEY_RIGHT])
    {
        textarea->caret_x += 1;
        
        KEYBOARD_STATE->key_locks[GLFW_KEY_RIGHT] = 1;
    }
    else
    if (KEYBOARD_STATE->keys[GLFW_KEY_UP] && !KEYBOARD_STATE->key_locks[GLFW_KEY_UP])
    {
        if (textarea->caret_y > 0)
            textarea->caret_y -= 1;
        
        KEYBOARD_STATE->key_locks[GLFW_KEY_UP] = 1;
    }
    else
    if (KEYBOARD_STATE->keys[GLFW_KEY_DOWN] && !KEYBOARD_STATE->key_locks[GLFW_KEY_DOWN])
    {
        if (textarea->caret_y >= textarea->nr_lines - 1)
            textarea_insert_new_line(textarea, textarea->caret_y, 0);

        textarea->caret_y += 1;
         
        KEYBOARD_STATE->key_locks[GLFW_KEY_DOWN] = 1;
    }
    else
    if (KEYBOARD_STATE->keys[GLFW_KEY_TAB] && !KEYBOARD_STATE->key_locks[GLFW_KEY_TAB])
    {
        textarea_on_character_input(textarea, KEYBOARD_STATE->character, "    ");
        KEYBOARD_STATE->key_locks[GLFW_KEY_TAB] = 1;
    }
    else
    if (KEYBOARD_STATE->character)
    { // alright, we are typing something.
        textarea_on_character_input(textarea, KEYBOARD_STATE->character, (void*) 0); 
    }

    textarea_reposition_caret(textarea);
    
    if (KEYBOARD_STATE->keys[GLFW_KEY_ENTER] && !KEYBOARD_STATE->key_locks[GLFW_KEY_ENTER])
    {
        textarea->caret_y += 1;

        textarea_insert_new_line(textarea, textarea->caret_y, 1);
            
        textarea->caret_x = 0;

        KEYBOARD_STATE->key_locks[GLFW_KEY_ENTER] = 1;
    }

    struct timeval end;
    gettimeofday(&end, 0);

    float time_spent = (double)(end.tv_usec - textarea->timer.tv_usec) / 1000000 + (double)(end.tv_sec - textarea->timer.tv_sec);

    if (time_spent >= 0.5f)
    {
        if (textarea->draw_caret)
        {
            textarea->draw_caret = 0;
        }
        else
        {
            textarea->draw_caret = 1;

        }

        gettimeofday(&textarea->timer, 0);
    }
}

void textarea_draw(actor_T* self)
{
    textarea_T* textarea = (textarea_T*) self;
    actor_focusable_T* actor_focusable = (actor_focusable_T*) textarea;
    state_T* state = (state_T*) get_current_scene();

    draw_positioned_2D_mesh(
        self->x,
        self->y,
        self->z,
        textarea->width,
        textarea->height,
        0, 0, 0, // rgb
        1.0f, // a
        state 
    );

    float scroll = 0;

    int available_space = textarea->width / (textarea->font_size + textarea->font_spacing);
    int chars_off = /*strlen(textarea->value)*/ textarea->caret_x - available_space;

    scroll = chars_off > 0 ? chars_off * (textarea->font_size + textarea->font_spacing) : 0;

    glEnable(GL_SCISSOR_TEST);
    glScissor((int)self->x, (int)(WINDOW_HEIGHT - self->y - textarea->height), (int)textarea->width, (int)textarea->height);

    for (int i = 0; i < textarea->nr_lines; i++)
    {
        char* line = textarea->lines[i];

        draw_text(
            line,
            self->x + (((textarea->font_size + textarea->font_spacing) / 2) - scroll),
            self->y + (textarea->font_size) + (i * (textarea->font_size + textarea->font_spacing)),
            self->z,
            255, 255, 255, // rgb
            1.0f, // a
            textarea->font_size,
            textarea->font_spacing,
            0,
            state
        );
    }

    glDisable(GL_SCISSOR_TEST);

    if (textarea->draw_caret && actor_focusable->focused)
    {
        draw_positioned_2D_mesh(
            self->x + ((textarea->font_size + textarea->font_spacing) * (float) textarea->caret_x - ((textarea->font_size + textarea->font_spacing) / 2)) + ((textarea->font_size + textarea->font_spacing) / 2) - scroll,
            self->y + (textarea->caret_y * (textarea->font_size + textarea->font_spacing)),
            self->z,
            4,
            (textarea->font_size * 2),
            255, 255, 255, // rgb
            1.0f,
            state       
        );
    } 
}

void textarea_on_character_input(textarea_T* textarea, char c, char* text)
{
    char* char_str = (void*) 0;

    if (text == (void*)0)
    {
        char_str = calloc(2, sizeof(char));
        char_str[0] = KEYBOARD_STATE->character;
        char_str[1] = '\0';
    }
    else
    {
        char_str = calloc(strlen(text) + 1, sizeof(char));
        strcpy(char_str, text);
    }

    int cx = textarea->caret_x;
    int cy = textarea->caret_y;

    if (textarea->nr_lines < cy)
    {
        for (int i = textarea->nr_lines; i < cy; i++)
        {
            textarea->nr_lines += 1;
            textarea->lines = realloc(textarea->lines, textarea->nr_lines * sizeof(char*));
            textarea->lines[textarea->nr_lines-1] = calloc(cx, sizeof(char));
            textarea->lines[textarea->nr_lines-1][0] = '\n';
            textarea->lines[textarea->nr_lines-1][1] = '\0';
        }
    }

    insert_substring(textarea->lines[cy], char_str, cx + 1);

    free(char_str);
    
    textarea->caret_x += 1;
}

void textarea_shift_down(textarea_T* textarea)
{
    int shiftpos = textarea->caret_y;

    if (textarea->caret_x < strlen(textarea->lines[textarea->caret_y - 1]) -1)
        shiftpos -= 1;

    for (int i = textarea->nr_lines-1; i > shiftpos; i--)
    {
        if (i + 1 > textarea->nr_lines)
            textarea->nr_lines += 1;
        
        textarea->lines[i] = calloc(strlen(textarea->lines[i-1]) + 1, sizeof(char));
        strcpy(textarea->lines[i], textarea->lines[i-1]);
        free(textarea->lines[i-1]);
        textarea->lines[i-1] = calloc(1, sizeof(char));
        textarea->lines[i-1][0] = '\0';
    }
}

void textarea_reposition_caret(textarea_T* textarea)
{
    size_t line_length = strlen(textarea->lines[textarea->caret_y]);

    if (textarea->caret_x > line_length)
        textarea->caret_x = line_length;
}

void textarea_insert_new_line(textarea_T* textarea, unsigned int position, unsigned int shift)
{
    textarea->nr_lines += 1;
    textarea->lines = realloc(textarea->lines, textarea->nr_lines * sizeof(char*));
    textarea->lines[textarea->nr_lines-1] = calloc(1, sizeof(char));
    textarea->lines[textarea->nr_lines-1][0] = '\0';

    if (shift && (textarea->caret_y < textarea->nr_lines-1))
        textarea_shift_down(textarea);
}
