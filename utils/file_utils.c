//
// Created by robert on 7/5/24.
//

#include "file_utils.h"
#include "../error.h"
#include "file.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define READ_FILE_BUFFER (1024 * 16)



int64_t get_file_size(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        return -1;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        return -1;
    }

    int64_t fileSize = ftell(file);
    if (fileSize == -1) {
        return -1;
    }

    fclose(file);
    return fileSize;
}

char* read_file(const char *filename) {
    int64_t file_size = get_file_size(filename);
    if(file_size == 0){
        return NULL;
    }

    FILE* f = fopen(filename, "r");
    if(f == NULL){
        return NULL;
    }

    char* file_content = xmalloc(sizeof(char) * (file_size + 1));
    memset(file_content, 0, sizeof(char) * (file_size + 1));

    char buffer[READ_FILE_BUFFER] = {0};
    size_t bytes_read = 0;
    while((bytes_read = fread(buffer, 1, READ_FILE_BUFFER, f)) > 0){
        buffer[bytes_read] = 0;
        strcat(file_content, buffer);
    }

    fclose(f);
    return file_content;
}

bool check_path(const char* path) {
    return strstr(path, "..") == NULL;
}

char* find_char_from_end(const char *str, char ch) {
    for (long i = strlen(str) - 1; i >= 0; i--) {
        if (str[i] == ch) {
            return (char*)(str + i);
        }
    }
    return NULL;
}