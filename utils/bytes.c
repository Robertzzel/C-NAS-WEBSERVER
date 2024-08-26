//
// Created by robert on 8/13/24.
//

#include "bytes.h"

bytes_t bytes_from(char* data, size_t size) {
    bytes_t res = {0};
    res.size = size;
    res.data = xmalloc(size);
    memcpy(res.data, data, size);
    return res;
}

void bytes_free(bytes_t* b) {
    free(b->data);
}

char* bytes_to_string(bytes_t* bytes) {
    char* str = xcalloc(bytes->size + 1);
    memcpy(str, bytes->data, bytes->size);
    return str;
}

int bytes_compare(bytes_t* b1, bytes_t* b2) {
    if(b1->size != b2->size) {
        return b1->size > b2->size ? 1 : -1;
    }
    for(size_t i = 0; i < b1->size; i++) {
        if(b1->data[i] != b2->data[i]) {
            return b1->data[i] > b2->data[i] ? 1 : -1;
        }
    }
    return 0;
}