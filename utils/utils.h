//
// Created by robert on 6/30/24.
//

#ifndef UNTITLED_UTILS_H
#define UNTITLED_UTILS_H

#include "string.h"
#include "stdlib.h"
#include "../error.h"
#include "stdio.h"
#include "stdbool.h"
#include <ctype.h>

#define STRING_ARRAY_INITIAL_DIMENSION 5

typedef struct {
    char** array;
    int size;
    int cap;
} list_string_t;

list_string_t* list_strings_t__new();
void list_strings_t__add(list_string_t* array, char* new_string, unsigned long length);
bool list_strings_t__replace(list_string_t* array, int index, char* new_string, unsigned long length);
bool list_strings_t__delete(list_string_t* array, int index);
char* list_strings_t__get(list_string_t* array, int index);
void list_strings_t__free(list_string_t* array);


void* xmalloc(size_t bytes);
void* xrealloc (void* pointer, size_t bytes);
char* xstrdup(const char* string);
char* xstrndup(const char* string, size_t length);

char* trim_whitespace(char *str);
list_string_t* string_split(char* string, char* delimiter);
char* toLower(char* s);



#endif //UNTITLED_UTILS_H
