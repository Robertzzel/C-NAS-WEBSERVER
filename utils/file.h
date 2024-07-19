//
// Created by robert on 7/14/24.
//

#ifndef UNTITLED_FILE_H
#define UNTITLED_FILE_H
#include "utils.h"

typedef struct {
    char name[256];
    char type;
} file_t;


void file_t__copy(file_t* another_file, file_t* another);

typedef struct {
    file_t* array;
    size_t size;
    size_t capacity;
} list_file_t;

list_file_t* list_file_t__new();

void list_file_t__insert(list_file_t* list, file_t* file);

file_t* list_file_t__get(list_file_t* list, size_t index);

void list_file_t__free(list_file_t* list);
char* list_file_t__to_json(file_t* file);
list_file_t* list_directory(const char* path);

#endif //UNTITLED_FILE_H
