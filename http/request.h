//
// Created by robert on 6/23/24.
//

#ifndef UNTITLED_REQUEST_H
#define UNTITLED_REQUEST_H

#include "stdlib.h"
#include "../error.h"
#include "../utils/utils.h"
#include "../IO/buffered_reader.h"
#include "../utils/bytes.h"

typedef struct {
    char* method;
    char* uri;
    char* version;
    char*** headers;
} request_t;

request_t* request_from_bytes(reader_t* reader);
void request_free(request_t *request);
char* request_header_value(request_t* request, char* name);
char* request_form_value(char* body, char* label);
#endif //UNTITLED_REQUEST_H
