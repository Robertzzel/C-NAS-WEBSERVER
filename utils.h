//
// Created by robert on 6/30/24.
//

#ifndef UNTITLED_UTILS_H
#define UNTITLED_UTILS_H

#include "string.h"
#include "stdlib.h"
#include "error.h"
#include "stdio.h"

#define STRING_ARRAY_INITIAL_DIMENSION 5

typedef struct {
    char** array;
    int size;
    int cap;
} list_strings_t;

error find_substring_index(const char *str, const char *substr, int* index);
error count_substring(const char *str, const char *substr, int* count);
error trim_whitespace(char *str, char** new_str, int* length);
error string_split(char* string, char* delimiter, list_strings_t* parts);

error list_strings_t__new(list_strings_t* array);
error list_strings_t__add(list_strings_t* array, char* new_string, unsigned long length);
error list_strings_t__replace(list_strings_t* array, int index, char* new_string, unsigned long length);
error list_strings_t__delete(list_strings_t* array, int index);
error list_strings_t__get(list_strings_t* array, int index, char** element);
error list_strings_t__free(list_strings_t* array);

#endif //UNTITLED_UTILS_H
