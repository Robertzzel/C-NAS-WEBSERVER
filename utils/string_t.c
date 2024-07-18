//
// Created by robert on 7/16/24.
//

#include <stdarg.h>
#include "string_t.h"

char* string__new(size_t length) {
    char* new_str = (char*)xmalloc(sizeof(char) * (length + 1));
    new_str[0] = 0;
    new_str[length] = 0;
    return new_str;
}

char* string__copy_n(char* str, size_t length) {
    char* result = xmalloc(sizeof(char) * (length + 1));
    strncpy(result, str, length);
    result[length] = 0;
    return result;
}

char* string__copy(char* str) {
    return string__copy_n(str, strlen(str));
}

char* string__append(char* first, char* second) {
    char* result = xmalloc(sizeof(char) * (strlen(first) + strlen(second) + 1));
    strcpy(result, first);
    strcat(result, second);
    return result;
}

char* string__append_n(char* first, char* second, size_t length) {
    char* result = xmalloc(sizeof(char) * (strlen(first) + length + 1));
    strcpy(result, first);
    strncat(result, second, length);
    return result;
}

list_string_t* string__split(char* str, char* delimiter) {
    list_string_t* parts = list_strings__new(2);

    char* substring = NULL;
    while((substring = strstr(str, delimiter)) != NULL) {
        list_strings__add_n(parts, str, substring - str);
        str = substring + strlen(delimiter);
    }

    list_strings__add(parts, str);
    return parts;
}

char* string__to_lower(char* str) {
    char* new_str = xmalloc(sizeof(char) * (strlen(str) + 1));
    for(size_t i = 0; i < strlen(str); i++){
        new_str[i] = (char)tolower(str[i]);
    }
    new_str[strlen(str)] = 0;
    return new_str;
}

int string__is_empty(char* this){
    return strcmp(this, "") == 0;
}

char* string__trim_whitespace(char *this) {
    char* str = this;
    char *end = this + strlen(this) - 1;

    while (*str == ' ') str++;
    while (end != str && *end == ' ') end--;

    char* new_str = string__copy_n(str, end - str);

    return new_str;
}

char* string__substring(char* this, uint32_t start_index, uint32_t end_index) {
    char* new_str = string__copy_n(this + start_index, end_index - start_index);
    return new_str;
}

char* string__concatenate_strings(int number_of_strings, ...) {
    va_list args;
    va_start(args, number_of_strings);

    size_t total_length = 0;
    for (int i = 0; i < number_of_strings; i++) {
        char *str = va_arg(args, char*);
        total_length += strlen(str);
    }

    char *result = xmalloc(total_length + 1);

    va_start(args, number_of_strings);
    result[0] = '\0'; // Initialize result with empty string
    for (int i = 0; i < number_of_strings; i++) {
        char *str = va_arg(args, char*);
        strcat(result, str);
    }

    va_end(args);
    return result;
}

list_string_t* list_strings__new(size_t initial_capacity){
    list_string_t* array = xmalloc(sizeof(list_string_t));
    array->cap = initial_capacity;
    array->array = xmalloc(sizeof(char*) * array->cap);
    array->size = 0;

    return array;
}

void list_strings__add(list_string_t* array, char* string) {
    if(array->size >= array->cap) {
        array->cap *= 2;
        array->array = xrealloc(array->array, sizeof(char*) * array->cap);
    }

    char* string_copy = string__copy(string);
    array->array[array->size] = string_copy;
    ++array->size;
}

void list_strings__add_n(list_string_t* array, char* string, size_t length) {
    if(array->size >= array->cap) {
        array->cap *= 2;
        array->array = xrealloc(array->array, sizeof(char*) * array->cap);
    }

    char* new_string = string__copy_n(string, length);
    array->array[array->size] = new_string;
    ++array->size;
}

void list_strings__replace(list_string_t* array, int index, char* new_string) {
    if(index >= array->size || index < 0) {
        fprintf (stderr, "index out of bounds\n");
        exit (1);
    }

    free(array->array[index]);
    char* string_copy = string__copy(new_string);
    array->array[index] = string_copy;
}

void list_strings__delete(list_string_t* array, int index) {
    if(index >= array->size || index < 0) {
        fprintf (stderr, "index out of bounds\n");
        exit (1);
    }

    free(array->array[index]);

    for(int i = index; i < array->size - 1; ++i) {
        array->array[i] = array->array[i+1];
    }

    --array->size;
    if(array->size <= array->cap / 4 && array->cap > 1) {
        array->cap /= 2;
        array->array = xrealloc(array->array, sizeof(char*) * array->cap);
    }
}

char* list_strings__get(list_string_t* array, int index) {
    if(index >= array->size || index < 0) {
        return NULL;
    }
    return array->array[index];
}

void list_strings__free(list_string_t* array){
    for(int i = 0; i< array->size; ++i){
        free(array->array[i]);
        array->array[i] = NULL;
    }
    if(array->array != NULL) {
        free(array->array);
        array->array = NULL;
    }
    free(array);
}

char* list_strings__to_string(list_string_t* list) {
    size_t total_length = 0;
    for(int i = 0; i < list->size; i++){
        total_length += strlen(list->array[i]);
    }

    char* new_str = string__new(total_length + 1);
    for(int i = 0; i < list->size; i++){
        strcat(new_str, list->array[i]);
    }

    return new_str;
}