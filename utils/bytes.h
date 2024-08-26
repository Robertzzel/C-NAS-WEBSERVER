//
// Created by robert on 8/13/24.
//

#ifndef UNTITLED_BYTES_H
#define UNTITLED_BYTES_H

#include "stdlib.h"
#include "utils.h"

typedef struct {
    char* data;
    size_t size;
} bytes_t;

bytes_t bytes_from(char* data, size_t size);
void bytes_free(bytes_t* b);
char* bytes_to_string(bytes_t* bytes);
int bytes_compare(bytes_t* b1, bytes_t* b2);

#endif //UNTITLED_BYTES_H
