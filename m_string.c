//
// Created by robert on 6/30/24.
//

#include "m_string.h"

error m_string_new(char** self, const char* text, unsigned long length) {
    *self = malloc(sizeof(char) * (length + 1));
    if(*self == NULL) {
        return FAIL;
    }
    if(length > 0){
        memcpy(*self, text, length);
    }
    *(*self + length) = 0;

    return SUCCESS;
}

error m_string_copy(char** this, char* another) {
    if(this == NULL || *this == NULL || another == NULL) {
        return FAIL;
    }
    unsigned long current_string_size = strlen(*this);
    unsigned long string_to_copy_size = strlen(another);
    if(current_string_size != string_to_copy_size) {
        *this = realloc(*this, string_to_copy_size + 1);
        if(*this == NULL){
            return FAIL;
        }
    }
    memcpy(*this, another, string_to_copy_size);
    (*this)[string_to_copy_size] = 0;
    return SUCCESS;
}

error m_string_append(char** this, char* another) {
    if(this == NULL || *this == NULL || another == NULL) {
        return FAIL;
    }

    unsigned long current_string_size = strlen(*this);
    unsigned long string_to_append_size = strlen(another);
    if(string_to_append_size <= 0) {
        return SUCCESS;
    }
    unsigned long new_string_size = current_string_size + string_to_append_size;
    *this = realloc(*this, sizeof(char) * (new_string_size + 1));
    if(*this == NULL){
        return FAIL;
    }
    memcpy(*this + current_string_size, another, string_to_append_size);
    (*this)[new_string_size] = 0;

    return SUCCESS;
}

error m_string_split(char* string, char* delimiter, char*** parts, int* number_of_parts) {
    error err = count_substring(string, delimiter, number_of_parts);
    if(err != SUCCESS){
        return err;
    }

    *parts = malloc(sizeof(char*) * *number_of_parts);
    if(*parts == NULL) {
        return FAIL;
    }

    int i = 0;
    int delimiter_index = -1;
    for(i = 0; i < *number_of_parts; ++i) {
        char* substring = strstr(string, delimiter);
        if(substring == NULL) {
            break;
        }

        *(*parts + i) = strndup(string, substring - string);
        if(*(*parts + i) == NULL){
            free(*parts);
            return FAIL;
        }

        string = substring + strlen(delimiter);
    }

    *number_of_parts = i;
    return SUCCESS;
}

error m_string_split2(char* string, char* delimiter, char** parts, int* number_of_parts) {
    error err = count_substring(string, delimiter, number_of_parts);
    if(err != SUCCESS){
        return err;
    }

    *parts = malloc(sizeof(char*) * *number_of_parts);
    if(*parts == NULL) {
        return FAIL;
    }

    int i = 0;
    int delimiter_index = -1;
    for(i = 0; i < *number_of_parts; ++i){
        err = find_substring_index(string, delimiter, &delimiter_index);
        if(err != SUCCESS){
            break;
        }
        err = m_string_new(parts+i, string, delimiter_index);
        if(err != SUCCESS) {
            break;
        }
        string += delimiter_index + strlen(delimiter);
    }

    *number_of_parts = i;
    return SUCCESS;
}

void string_array_free(char** string_array, int elements) {
    int i;
    for(i = 0; i < elements; ++i){
        free(string_array[i]);
        string_array[i] = NULL;
    }
    free(string_array);
    string_array = NULL;
}