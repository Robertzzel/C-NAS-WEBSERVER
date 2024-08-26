//
// Created by robert on 7/16/24.
//

#include <stdarg.h>
#include "string_t.h"

char** string__split(char* str, char* delimiter) {
    int nr_strings = 0;
    char* aux = str;
    while ((aux = strstr(aux+1, delimiter)) != NULL) {
        nr_strings++;
    }

    char** strings = xcalloc((nr_strings + 1) * sizeof(char*));

    char* substring_end = NULL;
    char* substring_start = str;
    size_t strings_index = 0;
    while((substring_end = strstr(str, delimiter)) != NULL) {
        strings[strings_index] = strndup(substring_start, substring_end - substring_start);
        substring_start = substring_end + strlen(delimiter);
        strings_index++;
    }

    return strings;
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

char* string__join(char** list, char* delim) {
    size_t total_length = 0;
    int i;
    for (i = 0; list[i] != NULL; i++) {
        total_length += strlen(list[i]);
    }

    total_length += strlen(delim) * (i-2);

    int last_string_index = i-2;
    char *new_str = xmalloc(total_length + 1);
    for (i = 0; list[i] != NULL; i++) {
        strcat(new_str, list[i]);
        if (i < last_string_index) {
            strcat(new_str, delim);
        }
    }

    return new_str;
}