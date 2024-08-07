//
// Created by robert on 7/15/24.
//

#include <dirent.h>
#include "file.h"

void file_t__copy(file_t* file, file_t* another) {
    strcpy(another->name, file->name);
    another->type = file->type;
}

list_file_t* list_file_t__new() {
    list_file_t* list = xmalloc(sizeof(list_file_t));
    list->capacity = 5;
    list->array = xmalloc(list->capacity * sizeof(file_t));
    list->size = 0;
    return list;
}

void list_file_t__insert(list_file_t* list, file_t* file) {
    if (list->size == list->capacity) {
        list->capacity *= 2;
        list->array = xrealloc(list->array, list->capacity * sizeof(file_t));
    }

    file_t__copy(file, &list->array[list->size]);
    list->size++;
}

file_t* list_file_t__get(list_file_t* list, size_t index) {
    if (index >= list->size) {
        fprintf(stderr, "Index out of bounds\n");
        exit(EXIT_FAILURE);
    }
    return &list->array[index];
}

void list_file_t__free(list_file_t* list) {
    free(list->array);
    list->array = NULL;
    list->size = 0;
    list->capacity = 0;
    free(list);
}

char* list_file_t__to_json(file_t* file) {
    char* type_string = string__from(&file->type, 1);
    char* result = string__concatenate_strings(6, "{\"name\":\"", file->name, "\",", "\"type\":\"", type_string, "\"}");
    free(type_string);
    return result;
}

list_file_t* list_directory(const char* path) {
    DIR *dir;
    struct dirent *entry;

    // Allocate initial memory for directory entries
    list_file_t* entries = list_file_t__new();

    if ((dir = opendir(path)) == NULL) {
        fprintf(stderr, "cannot list directory %s\n", path);
        free(entries);
        return NULL;
    }

    while ((entry = readdir(dir)) != NULL) {
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        file_t file;
        if(strlen(entry->d_name) < 256) {
            strcpy(file.name, entry->d_name);
        }

        if (entry->d_type == DT_DIR) {
            file.type = 'd';
        } else if (entry->d_type == DT_REG) {
            file.type = 'f';;
        } else {
            file.type = 'o';;
        }

        list_file_t__insert(entries, &file);
    }

    closedir(dir);
    return entries;
}