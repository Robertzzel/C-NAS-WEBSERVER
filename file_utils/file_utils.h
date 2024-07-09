//
// Created by robert on 7/5/24.
//

#ifndef UNTITLED_FILE_UTILS_H
#define UNTITLED_FILE_UTILS_H

#include <dirent.h>
#include <sys/stat.h>
#include "stdio.h"
#include "stdint.h"
#include "zip.h"
#include "../socket_t.h"
#include "../utils.h"

error read_file(const char *filename, char** file_content);
error get_file_size(const char *filename, uint64_t* file_size);
error check_path(const char* path);
char* find_char_from_end(const char *str, char ch);

#endif //UNTITLED_FILE_UTILS_H
