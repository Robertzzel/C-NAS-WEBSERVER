//
// Created by robert on 7/15/24.
//

#include "file.h"


file_t* file_t__new(char* name) {
    file_t* file = xmalloc(sizeof(file_t));
    file->name = xstrdup(name);
    return file;
}

void file_t__free(file_t* file) {
    free(file->name);
    free(file);
}

void file_t__copy(file_t* file, file_t* another) {
    another->name = xstrdup(file->name);
}
list_file_t* list_file_t__new() {
    list_file_t* list = xmalloc(sizeof(list_file_t));
    list->capacity = 5;
    list->array = xmalloc(list->capacity * sizeof(file_t*));
    list->size = 0;
    return list;
}

void list_file_t__insert(list_file_t* list, file_t* file) {
    if (list->size == list->capacity) {
        list->capacity *= 2;
        list->array = xrealloc(list->array, list->capacity * sizeof(file_t*));
    }

    list->array[list->size] = xmalloc(sizeof(file_t));
    file_t__copy(file, list->array[list->size]);
    list->size++;
}

file_t* list_file_t__get(list_file_t* list, size_t index) {
    if (index >= list->size) {
        fprintf(stderr, "Index out of bounds\n");
        exit(EXIT_FAILURE);
    }
    return list->array[index];
}

void list_file_t__free(list_file_t* list) {
    for(int i=0;i<list->size;i++){
        file_t__free(list->array[i]);
    }
    free(list->array);
    list->array = NULL;
    list->size = 0;
    list->capacity = 0;
    free(list);
}

char* list_file_t__to_json(file_t* file) {
    // Convert the name field to JSON
    size_t length = strlen(file->name);
    char* json_name = (char*)malloc(length + 3); // 2 quotes + null terminator
    sprintf(json_name, "\"%s\"", file->name);

    // Calculate the total length of the final JSON string
    size_t totalLength = strlen(json_name) + 10; // 10 is an arbitrary extra space for the JSON structure

    // Allocate memory for the final JSON string
    char* json = (char*)malloc(totalLength);

    // Construct the final JSON string
    sprintf(json, "{\"name\":%s}", json_name);

    // Free the temporary JSON string
    free(json_name);

    return json;
}