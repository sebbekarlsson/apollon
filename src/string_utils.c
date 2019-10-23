#include "include/string_utils.h"
#include <string.h>
#include <stdlib.h>


/**
 * Returns a copy of a const char* value
 *
 * @param const char* value
 *
 * @return char*
 */
char* string_copy(const char* value)
{
    char* new_value = calloc(strlen(value) + 1, sizeof(char));
    strcpy(new_value, value);

    return new_value;
}
