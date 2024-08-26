//
// Created by robert on 7/9/24.
//

#ifndef UNTITLED_ZIP_H
#define UNTITLED_ZIP_H

#include "../IO/socket_t.h"
#include "utils.h"
#include "../error.h"
#include "stdbool.h"
#include "../IO/buffered_reader.h"
/*
 ZIP FILE STRUCTURE:
    LOCAL_FILE_HEADER1
    FILE_CONTENT1
    LOCAL_FILE_HEADER2
    FILE_CONTENT2
    ...
    CENTRAL_DIRECTORY_HEADER1
    CENTRAL_DIRECTORY_HEADER2
    ...
    END OF CENTRAL DIRECTORY HEADER
    LOCATOR
    ZIP DIRECTORY END
 */

#define ZIP_LOCAL_FILE_HEADER_SIGNATURE 0x04034b50
#define ZIP_CENTRAL_DIRECTORY_HEADER_SIGNATURE 0x02014b50
#define ZIP_END_OF_CENTRAL_DIRECTORY_SIGNATURE 0x06064b50
#define ZIP_LOCATOR_SIGNATURE 0x07064b50

#define LOCAL_FILE_HEADER_SIZE 30
#define CENTRAL_DIRECTORY_HEADER_SIZE 46
#define ZIP_LOCATOR_SIZE 20
#define ZIP_DIRECTORY_END_SIZE 22
#define ZIP_END_OF_CENTRAL_DIRECTORY 56

#define READ_FILE_BUFFER_SIZE (1024 * 8)


bool write_zip_to_socket(char** file_paths, reader_t* socket);

#endif //UNTITLED_ZIP_H
