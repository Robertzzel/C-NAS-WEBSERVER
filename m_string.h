//
// Created by robert on 6/30/24.
//

#ifndef UNTITLED_M_STRING_H
#define UNTITLED_M_STRING_H

#include "stdlib.h"
#include "string.h"
#include "utils.h"
#include "error.h"

error m_string_new(char** self, const char* text, unsigned long length);
error m_string_copy(char** this, char* another);
error m_string_append(char** this, char* another);
error m_string_split(char* string, char* delimiter, char*** parts, int* number_of_parts);
void string_array_free(char** string_array, int elements);

#endif //UNTITLED_M_STRING_H
