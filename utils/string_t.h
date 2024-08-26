//
// Created by robert on 7/16/24.
//

#ifndef UNTITLED_STRING_T_H
#define UNTITLED_STRING_T_H
#include "inttypes.h"
#include "utils.h"
#include "string_t.h"
#include <string.h>

char* string__join(char** strings, char* delim);
char** string__split(char* str, char* delimiter);
char* string__concatenate_strings(int number_of_strings, ...);

#endif //UNTITLED_STRING_T_H
