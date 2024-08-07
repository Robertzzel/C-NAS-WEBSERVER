//
// Created by robert on 7/16/24.
//

#ifndef UNTITLED_STRING_T_H
#define UNTITLED_STRING_T_H
#include "inttypes.h"
#include "utils.h"
#include "string_t.h"
#include <string.h>
#include "../functional_utils/list.h"

typedef struct {
    char **array;
    size_t size;
    size_t cap;
} list_string_t;

char* string__new(size_t length);
char* string__from(char* str, size_t length);
char* string__append(char* this, char* another, size_t length);
char* string__to_lower(char* str);
int string__is_empty(char* this);
char* string__trim_whitespace(char* this);
char* string__substring(char* this, uint32_t start_index, uint32_t end_index);
char* string__concatenate_strings(int number_of_strings, ...);
list_string_t* string__split(char* this, char* delimiter);
char* string__join(list_string_t* list, char* delim);

list_string_t* list_strings__new(size_t initial_capacity);
void list_strings__add(list_string_t* array, char* string);
void list_strings__add_n(list_string_t* array, char* string, size_t length);
void list_strings__replace(list_string_t* array, int index, char* string);
void list_strings__delete(list_string_t* array, int index);
char* list_strings__get(list_string_t* array, int index);
void list_strings__free(list_string_t* array);

#endif //UNTITLED_STRING_T_H
