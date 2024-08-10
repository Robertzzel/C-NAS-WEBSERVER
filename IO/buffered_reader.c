//
// Created by robert on 8/8/24.
//

#include "buffered_reader.h"

#define MIN(x,y) ((x) > (y) ? (y) : (x))

int fill(buffered_socket_t* reader) {
    if (reader->read > 0) {
        memmove(reader->buffer, reader->buffer + reader->read, reader->write - reader->read);
        reader->read = 0;
        reader->write -= reader->read;
    }
    if (reader->write >= reader->buffer_size) {
        return 1; // buffer already full
    }
    int bytes_read = socket__read_timeout(
        reader->socket,
        reader->buffer + reader->write,
        reader->buffer_size - reader->write
    );
    if(bytes_read < 0){
        return 0;
    }
    reader->write += bytes_read;
}

size_t buffered(buffered_socket_t* reader) {
    return reader->write - reader->read;
}

buffered_socket_t* buffered_socket_new(socket_t* s) {
    buffered_socket_t* buff = xmalloc(sizeof(buffered_socket_t));
    buff->socket = s;
    buff->buffer_size = DEFAULT_BUFFER_SIZE;
    buff->buffer = xmalloc(buff->buffer_size);
    return buff;
}

void buffered_socket_free(buffered_socket_t* reader) {
    free(reader->buffer);
    free(reader);
}

LIST(char)* buffered_socket_read(buffered_socket_t* reader, size_t max_read) {
    size_t number_of_buffered_bytes = buffered(reader);
    if(number_of_buffered_bytes <= 0) {
        fill(reader);
    }

    number_of_buffered_bytes = buffered(reader);
    size_t returned_list_size = number_of_buffered_bytes > max_read ? max_read : number_of_buffered_bytes;
    if(returned_list_size <= 0) {
        return NULL;
    }

    LIST(char)* r = LIST_NEW(char, 0, returned_list_size);
    LIST_ADD_MULTIPLE(r, reader->buffer + reader->read, returned_list_size);

    reader->read += returned_list_size;
    return r;
}

LIST(char)* buffered_socket_read_until_or_max(buffered_socket_t* reader, char delim, size_t max_read) {
    size_t buffered_bytes = buffered(reader);
    if(buffered_bytes <= 0){
        fill(reader);
    }

    buffered_bytes = buffered(reader);
    size_t returned_list_size = MIN(buffered_bytes, max_read);
    if(returned_list_size <= 0) {
        return NULL;
    }

    int delim_index = -1;
    for (size_t i = 0; i < returned_list_size; ++i) {
        if (reader->buffer[reader->read+i] == delim){
            delim_index = i;
        }
    }
    if(delim_index != -1){
        returned_list_size = delim_index + 1;
    }

    LIST(char)* r = LIST_NEW(char, 0, returned_list_size);
    LIST_ADD_MULTIPLE(r, reader->buffer + reader->read, returned_list_size);

    reader->read += returned_list_size;
    return r;
}
