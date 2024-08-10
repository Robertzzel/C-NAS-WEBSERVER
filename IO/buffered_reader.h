//
// Created by robert on 8/8/24.
//

#ifndef UNTITLED_BUFFERED_READER_H
#define UNTITLED_BUFFERED_READER_H

#define DEFAULT_BUFFER_SIZE 4096

#include "socket_t.h"

typedef struct {
    socket_t* socket;
    char* buffer;
    size_t read;
    size_t write;
    size_t buffer_size;
} buffered_socket_t;

buffered_socket_t* buffered_socket_new(socket_t* s);
void buffered_socket_free(buffered_socket_t* reader);
LIST(char)* buffered_socket_read(buffered_socket_t* reader, size_t max_read);
LIST(char)* buffered_socket_read_until_or_max(buffered_socket_t* reader, char delim, size_t max_read) ;

#endif //UNTITLED_BUFFERED_READER_H
