//
// Created by robert on 7/24/24.
//

#ifndef UNTITLED_LIST_H
#define UNTITLED_LIST_H

#include <stdlib.h>
#include "../utils/utils.h"

typedef struct {
    void *data;
    size_t elementSize;
    size_t size;
    size_t capacity;
} list_t;

list_t list_new(size_t elementSize, size_t cap);
void list_append(list_t* list, void *item);
void* list_get(list_t* list, int index);
size_t list_size(list_t* list);
size_t list_cap(list_t* list);

#endif //UNTITLED_LIST_H
