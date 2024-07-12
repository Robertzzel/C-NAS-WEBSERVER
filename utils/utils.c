//
// Created by robert on 6/30/24.
//

#include "utils.h"

char* toLower(char* s) {
    size_t input_size = strlen(s);
    char* temp = xmalloc(sizeof(char) * (input_size + 1));
    for(char *p=s; *p; p++) *temp=(char)tolower(*p);

    temp[strlen(s)] = 0;
    return temp;
}

char* trim_whitespace(char *str) {
    size_t str_length = strlen(str);
    char *end = str + str_length - 1;

    while (*str == ' ') str++;
    while (end != str && *end == ' ') end--;

    long result_size = end - str;
    char* result = xmalloc(sizeof(char) * (result_size + 1));
    strncpy(result, str, result_size);

    return result;
}

list_string_t* string_split(char* string, char* delimiter) {
    list_string_t* parts = list_strings_t__new();

    char* substring = NULL;
    while((substring = strstr(string, delimiter)) != NULL) {
        list_strings_t__add(parts, string, substring - string);
        string = substring + strlen(delimiter);
    }
    list_strings_t__add(parts, string, strlen(string));
    return parts;
}

list_string_t* list_strings_t__new(){
    list_string_t* array = xmalloc(sizeof(list_string_t));
    array->array = xmalloc(sizeof(char*) * STRING_ARRAY_INITIAL_DIMENSION);
    array->size = 0;
    array->cap = 0;
    return array;
}

void list_strings_t__add(list_string_t* array, char* new_string, unsigned long length) {
    if(array->size >= array->cap) {
        array->array = xrealloc(array->array, sizeof(char*) * (array->cap + STRING_ARRAY_INITIAL_DIMENSION));
        array->cap += STRING_ARRAY_INITIAL_DIMENSION;
    }
    array->array[array->size] = xstrndup(new_string, length);
    ++array->size;
}

bool list_strings_t__replace(list_string_t* array, int index, char* new_string, unsigned long length) {
    if(index >= array->size || index < 0) {
        return false;
    }

    free(array->array[index]);
    array->array[index] = xstrndup(new_string, length);

    return true;
}

bool list_strings_t__delete(list_string_t* array, int index) {
    if(index >= array->size || index < 0) {
        return false;
    }

    free(array->array[index]);

    int i;
    for(i = index; i < array->size - 1; ++i) {
        array->array[i] = array->array[i+1];
    }

    --array->size;
    if(array->cap - array->size > STRING_ARRAY_INITIAL_DIMENSION) {
        array->array = xrealloc(array->array, sizeof(char*) * (array->cap + STRING_ARRAY_INITIAL_DIMENSION));
        array->cap -= STRING_ARRAY_INITIAL_DIMENSION;
    }

    return true;
}

char* list_strings_t__get(list_string_t* array, int index) {
    if(index >= array->size || index < 0) {
        return NULL;
    }
    return array->array[index];
}

void list_strings_t__free(list_string_t* array){
    int i;
    for(i=0; i< array->size; ++i){
        if(array->array[i] != NULL){
            free(array->array[i]);
            array->array[i] = NULL;
        }
    }
    if(array->array != NULL) {
        free(array->array);
        array->array = NULL;
    }
    free(array);
}


void memory_error_and_abort (char *fname)
{
    fprintf (stderr, "%s: out of virtual memory\n", fname);
    exit (2);
}

void* xmalloc(size_t bytes) {
    void *temp = malloc (bytes);
    if (temp == 0)
        memory_error_and_abort ("xmalloc");
    return temp;
}

void* xrealloc (void* pointer, size_t bytes)
{
    void* temp = pointer ? realloc (pointer, bytes) : malloc (bytes);
    if (temp == 0)
        memory_error_and_abort ("xrealloc");
    return temp;
}

char* xstrdup(const char* string) {
    char *temp = strdup(string);
    if (temp == 0)
        memory_error_and_abort ("xstrdump");
    return temp;
}

char* xstrndup(const char* string, size_t length) {
    char *temp = strndup(string, length);
    if (temp == 0)
        memory_error_and_abort ("xstrndump");
    return temp;
}