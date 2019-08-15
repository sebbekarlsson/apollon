#ifndef APOLLON_STRING_HELPERS_H
#define APOLLON_STRING_HELPERS_H

char* str_get_left(char* str, int position);

char* str_get_right(char* str, int position);

char* substring(char *string, int position, int length);

void insert_substring(char *a, char *b, int position);

void str_erase(char** str, int position);
#endif
