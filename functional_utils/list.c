//
// Created by robert on 8/8/24.
//
#include "list.h"

list_t list_new(size_t elementSize, size_t cap) {
    list_t list;
    list.size = 0;
    list.capacity = cap;
    list.elementSize = elementSize;
    list.data = xmalloc(elementSize * cap);
    return list;
}

void list_append(list_t* list, void *item) {
    if (list->size >= list->capacity) {
        list->capacity *= 2;
        list->data = xrealloc(list->data, list->elementSize * list->capacity);
    }
    memcpy(list->data + list->size * list->elementSize, item, list->elementSize);
    list->size++;
}

void* list_get(list_t* list, int index) {
    return list->data + index * list->elementSize;
}

size_t list_size(list_t* list) {
    return list->size;
}

size_t list_cap(list_t* list) {
    return list->capacity;
}