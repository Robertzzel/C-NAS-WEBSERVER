//
// Created by robert on 7/5/24.
//

#include <string.h>
#include "file_utils.h"
#include "../error.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define READ_FILE_BUFFER (1024 * 16)

error get_file_size(const char *filename, uint64_t* file_size) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        return FAIL;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        return FAIL;
    }

    uint64_t fileSize = ftell(file);
    if (fileSize == -1) {
        return FAIL;
    }

    *file_size = fileSize;

    fclose(file);
    return SUCCESS;
}

error read_file(const char *filename, char** file_content) {
    uint64_t file_size = 0;
    error err = get_file_size(filename, &file_size);
    if(err != SUCCESS){
        return err;
    }

    *file_content = calloc(sizeof(char), file_size + 1);
    if(*file_content == NULL){
        return FAIL;
    }

    FILE* f = fopen(filename, "r");
    if(f == NULL){
        return FAIL;
    }

    char buffer[READ_FILE_BUFFER] = {0};
    size_t bytes_read = 0;
    while((bytes_read = fread(buffer, 1, READ_FILE_BUFFER, f)) > 0){
        strcat(*file_content, buffer);
    }

    fclose(f);
    return SUCCESS;
}

error check_path(const char* path) {
    return strstr(path, "..") == NULL ? SUCCESS : FAIL;
}

char* find_char_from_end(const char *str, char ch) {
    for (long i = strlen(str) - 1; i >= 0; i--) {
        if (str[i] == ch) {
            return (char*)(str + i);
        }
    }
    return NULL;
}