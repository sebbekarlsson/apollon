#include "include/string_helpers.h"
#include <string.h>
#include <stdlib.h>


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

char* substring(char *string, int position, int length)
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

void insert_substring(char *a, char *b, int position)
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
