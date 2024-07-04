//
// Created by robert on 7/3/24.
//

#include "html_files.h"

error get_file_size(FILE* f, long* size) {
    if(f == NULL || size == NULL){
        return FAIL;
    }
    long initial_size = ftell(f);
    fseek(f, 0, SEEK_END); // seek to end of file
    *size = ftell(f); // get current file pointer
    fseek(f, initial_size, SEEK_SET);
    return SUCCESS;
}

error get_home_page(char** file_content) {
    FILE *f = fopen(HOME_PAGE_FILE_URL, "r");
    if(f == NULL){
        return FAIL;
    }

    long file_size;
    error err = get_file_size(f, &file_size);
    if(err != SUCCESS){
        return err;
    }

    *file_content = malloc(sizeof(char) * (file_size + 1));
    if(*file_content == NULL){
        return FAIL;
    }

    char buffer[64];
    while(fgets(buffer, 63, f) != NULL) {
        strcat(*file_content, buffer);
    }

    return SUCCESS;
}