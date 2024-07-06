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
#include "../utils.h"

error write_zip_file(const char *zip_filename, array_of_strings_t* files);
error write_zip_to_socket(array_of_strings_t* files, s_socket* socket);

#endif //UNTITLED_FILE_UTILS_H
