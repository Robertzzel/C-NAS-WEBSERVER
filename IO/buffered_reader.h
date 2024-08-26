//
// Created by robert on 8/8/24.
//

#ifndef UNTITLED_BUFFERED_READER_H
#define UNTITLED_BUFFERED_READER_H

#define DEFAULT_BUFFER_SIZE 4096

#include "socket_t.h"
#include "../utils/bytes.h"

typedef struct {
    socket_t* socket;
    char* buffer;
    size_t read;
    size_t write;
    size_t buffer_size;
} reader_t;

reader_t* reader_new(socket_t* s);
void reader_free(reader_t* reader);
bytes_t* reader_read(reader_t* reader, size_t max_read);
bytes_t* reader_read_until(reader_t* reader, char delim, size_t max_read);
int reader_write(reader_t* reader, bytes_t* bytes);
int reader_write_buffer(reader_t* reader, void* bytes, size_t nr);

#endif //UNTITLED_BUFFERED_READER_H
