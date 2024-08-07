//
// Created by robert on 6/30/24.
//

#ifndef UNTITLED_UTILS_H
#define UNTITLED_UTILS_H

#include "string_t.h"
#include "stdlib.h"
#include "stdio.h"
#include <ctype.h>

void* xmalloc(size_t bytes);
void* xcalloc(size_t bytes);
void* xrealloc (void* pointer, size_t bytes);
char* xstrdup(const char* string);
char* xstrndup(const char* string, size_t length);

#endif //UNTITLED_UTILS_H
