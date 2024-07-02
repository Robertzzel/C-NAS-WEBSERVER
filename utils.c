//
// Created by robert on 6/30/24.
//

#include "utils.h"

error find_substring_index(const char *str, const char *substr, int* index) {
    char *pos = strstr(str, substr);
    if (pos == NULL) {
        return FAIL;
    }
    *index = pos - str;
    return SUCCESS;
}

error count_substring(const char *str, const char *substr, int* occurrences) {
    *occurrences = 0;
    int next_delimiter_index = 0;
    error err = find_substring_index(str, substr, &next_delimiter_index);
    while (err == SUCCESS) {
        (*occurrences)++;
        str += next_delimiter_index + 1;
        err = find_substring_index(str, substr, &next_delimiter_index);
    }
    return SUCCESS;
}

error get_substring_indexes(const char *str, const char *substr, int** indexes, int* number_of_indexes) {
    error err = count_substring(str, substr, number_of_indexes);
    if(err != SUCCESS){
        return err;
    }

    *indexes = malloc(sizeof(int) * *number_of_indexes);
    if(*indexes == NULL){
        return FAIL;
    }

    int i;
    int delimiter_index = -1;
    int x = 0;
    for(i = 0; i < *number_of_indexes; ++i) {
        err = find_substring_index(str, substr, &delimiter_index);
        if(err != SUCCESS){
            return err;
        }
        *(*indexes + i) = x + delimiter_index;
        x += delimiter_index + strlen(substr);
        str += delimiter_index + strlen(substr);
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