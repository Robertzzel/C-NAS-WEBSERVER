//
// Created by robert on 6/23/24.
//

#ifndef UNTITLED_HTTP_REQUEST_H
#define UNTITLED_HTTP_REQUEST_H

#include "stdlib.h"
#include "../error.h"
#include "../utils/utils.h"
#include "../IO/buffered_reader.h"

typedef struct {
    LIST(char)* method;
    LIST(char)* uri;
    LIST(char)* version;
    LIST(list_char)* header_names;
    LIST(list_char)* headers_values;
    LIST(char)* body;
} request_t;

request_t* request_from_bytes(buffered_socket_t* reader);
void http_request_t__free(request_t *request);
char* http_request_t___get_form_value(request_t* request, char* key);
#endif //UNTITLED_HTTP_REQUEST_H
