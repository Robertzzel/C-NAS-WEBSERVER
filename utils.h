//
// Created by robert on 6/30/24.
//

#ifndef UNTITLED_UTILS_H
#define UNTITLED_UTILS_H

#include "m_string.h"
#include "error.h"

error find_substring_index(const char *str, const char *substr, int* index);
error count_substring(const char *str, const char *substr, int* count);
error trim_whitespace(char *str, char** new_str, int* length);
error get_substring_indexes(const char *str, const char *substr, int** indexes, int* number_of_indexes);

#endif //UNTITLED_UTILS_H
