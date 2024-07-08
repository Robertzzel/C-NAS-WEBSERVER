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

error write_zip_file(const char *zip_filename, list_strings_t* files);
error write_zip_to_socket(list_strings_t* files, socket_t* socket);

#endif //UNTITLED_FILE_UTILS_H
