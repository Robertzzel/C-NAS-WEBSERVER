//
// Created by robert on 6/30/24.
//

#ifndef UNTITLED_UTILS_H
#define UNTITLED_UTILS_H

#include "string.h"
#include "stdlib.h"
#include "error.h"

#define STRING_ARRAY_INITIAL_DIMENSION 5

typedef struct {
    char** array;
    int size;
    int cap;
} array_of_strings_t;

error find_substring_index(const char *str, const char *substr, int* index);
error count_substring(const char *str, const char *substr, int* count);
error trim_whitespace(char *str, char** new_str, int* length);
error string_split(char* string, char* delimiter, array_of_strings_t* parts);

error string_array_new(array_of_strings_t* array);
error string_array_add(array_of_strings_t* array, char* new_string, unsigned long length);
error string_array_replace(array_of_strings_t* array, int index, char* new_string, unsigned long length);
error string_array_delete(array_of_strings_t* array, int index);
error string_array_get(array_of_strings_t* array, int index, char** element);
error string_array_free(array_of_strings_t* array);

#endif //UNTITLED_UTILS_H
