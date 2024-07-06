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
#include "../s_socket.h"

void zip_directory(const char *directory, const char *zip_filename);
void add_file_to_zip(zip_t *zip, const char *filename, const char *entryname);

void write_zip_file(const char *zip_filename, const char **files, int file_count);
#endif //UNTITLED_FILE_UTILS_H
