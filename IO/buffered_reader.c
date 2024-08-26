//
// Created by robert on 8/8/24.
//

#include "buffered_reader.h"

#define MIN(x,y) ((x) > (y) ? (y) : (x))

int fill(reader_t* reader) {
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

size_t buffered(reader_t* reader) {
    return reader->write - reader->read;
}

reader_t* reader_new(socket_t* s) {
    reader_t * buff = xmalloc(sizeof(reader_t));
    buff->socket = s;
    buff->buffer_size = DEFAULT_BUFFER_SIZE;
    buff->buffer = xmalloc(buff->buffer_size);
    return buff;
}

void reader_free(reader_t* reader) {
    free(reader->buffer);
    free(reader);
}

bytes_t* reader_read(reader_t* reader, size_t max_read) {
    size_t number_of_buffered_bytes = buffered(reader);
    if(number_of_buffered_bytes <= 0) {
        fill(reader);
    }

    number_of_buffered_bytes = buffered(reader);
    size_t returned_list_size = number_of_buffered_bytes > max_read ? max_read : number_of_buffered_bytes;
    if(returned_list_size <= 0) {
        return NULL;
    }

    bytes_t* r = xmalloc(sizeof(bytes_t));
    r->data = xmalloc(returned_list_size);
    r->size = returned_list_size;
    memcpy(r->data, reader->buffer + reader->read, returned_list_size);

    reader->read += returned_list_size;
    return r;
}

bytes_t* reader_read_until(reader_t* reader, char delim, size_t max_read) {
    size_t buffered_bytes = buffered(reader);
    if(buffered_bytes <= 0){
        fill(reader);
        buffered_bytes = buffered(reader);
    }

    size_t returned_list_size = MIN(buffered_bytes, max_read);
    if(returned_list_size <= 0) {
        return NULL;
    }

    int delim_index = -1;
    for (size_t i = 0; i < returned_list_size; ++i) {
        if (reader->buffer[reader->read+i] == delim){
            delim_index = i;
            break;
        }
    }
    if(delim_index != -1){
        returned_list_size = delim_index + 1;
    }

    bytes_t* r = xmalloc(sizeof(bytes_t));
    r->data = xmalloc(returned_list_size);
    r->size = returned_list_size;
    memcpy(r->data, reader->buffer + reader->read, returned_list_size);

    reader->read += returned_list_size;
    return r;
}

int reader_write(reader_t* reader, bytes_t* bytes) {
    return socket__write(reader->socket, bytes->data, bytes->size);
}

int reader_write_buffer(reader_t* reader, void* bytes, size_t nr) {
    return socket__write(reader->socket, bytes, nr);
}