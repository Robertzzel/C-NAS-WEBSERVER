//
// Created by robert on 6/30/24.
//

#include "utils.h"

error get_file_size(FILE* f, long* size) {
    if(f == NULL || size == NULL){
        return FAIL;
    }
    long initial_size = ftell(f);
    fseek(f, 0, SEEK_END); // seek to end of file
    *size = ftell(f); // get current file pointer
    fseek(f, initial_size, SEEK_SET);
    return SUCCESS;
}

error count_substring(const char *str, const char *substr, int* occurrences) {
    *occurrences = 0;
    char* substring = strstr(str, substr);
    while (substring != NULL) {
        (*occurrences)++;
        str += substring - str + 1;
        substring = strstr(str, substr);
    }
    return SUCCESS;
}

error trim_whitespace(char *str, char** new_str, int* length) {
    unsigned long str_length = strlen(str);
    char *end = str + str_length - 1;
    // Trim leading space
    while (*str == ' ') str++;
    while (end != str && *end == ' ') end--;

    *new_str = str;
    *length = end - str;

    return SUCCESS;
}

error string_split(char* string, char* delimiter, array_of_strings_t* parts) {
    error err = string_array_new(parts);
    if(err != SUCCESS){
        return err;
    }

    char* substring = NULL;
    while((substring = strstr(string, delimiter)) != NULL) {
        err = string_array_add(parts, string, substring-string);
        if(err != SUCCESS){
            string_array_free(parts);
            return err;
        }
        string = substring + strlen(delimiter);
    }
    return SUCCESS;
}

error string_array_new(array_of_strings_t* array){
    array->array = malloc(sizeof(char*) * STRING_ARRAY_INITIAL_DIMENSION);
    if(array->array == NULL) {
        return FAIL;
    }
    array->size = 0;
    array->cap = STRING_ARRAY_INITIAL_DIMENSION;
    return SUCCESS;
}

error string_array_add(array_of_strings_t* array, char* new_string, unsigned long length) {
    if(array->size >= array->cap) {
        array->array = realloc(array->array, sizeof(char*) * (array->cap + STRING_ARRAY_INITIAL_DIMENSION));
        if(array->array == NULL){
            return FAIL;
        }
        array->cap += STRING_ARRAY_INITIAL_DIMENSION;
    }
    array->array[array->size] = strndup(new_string, length);
    if(array->array[array->size] == NULL){
        return FAIL;
    }
    ++array->size;
    return SUCCESS;
}

error string_array_replace(array_of_strings_t* array, int index, char* new_string, unsigned long length) {
    if(index >= array->size || index < 0) {
        return FAIL;
    }

    free(array->array[index]);
    array->array[index] = strndup(new_string, length);
    if(array->array[index] == NULL){
        return FAIL;
    }

    return SUCCESS;
}

error string_array_delete(array_of_strings_t* array, int index) {
    if(index >= array->size || index < 0) {
        return FAIL;
    }

    free(array->array[index]);

    int i;
    for(i = index; i < array->size - 1; ++i) {
        array->array[i] = array->array[i+1];
    }

    --array->size;
    if(array->cap - array->size > STRING_ARRAY_INITIAL_DIMENSION) {
        array->array = realloc(array->array, sizeof(char*) * (array->cap + STRING_ARRAY_INITIAL_DIMENSION));
        if(array->array == NULL){
            return FAIL;
        }
        array->cap -= STRING_ARRAY_INITIAL_DIMENSION;
    }

    return SUCCESS;
}

error string_array_get(array_of_strings_t* array, int index, char** element) {
    if(index >= array->size || index < 0) {
        return FAIL;
    }
    *element = array->array[index];
    return SUCCESS;
}

error string_array_free(array_of_strings_t* array){
    int i = 0;
    for(i=0; i< array->size; ++i){
        free(array->array[i]);
    }
    if(array->array != NULL) {
        free(array->array);
    }
    return SUCCESS;
}