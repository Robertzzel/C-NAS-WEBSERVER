//
// Created by robert on 6/23/24.
//

#ifndef UNTITLED_HTTP_REQUEST_H
#define UNTITLED_HTTP_REQUEST_H

#include "stdlib.h"
#include "../error.h"
#include "../utils/utils.h"
#define HTTP_MESSAGE_DELIMITER "\r\n"

typedef struct {
    char* method;
    char* uri;
    char* version;
    list_string_t* header_names;
    list_string_t* headers_values;
    char* body;
} http_request_t;

http_request_t* http_request_t__from_bytes(char* message);
void http_request_t__free(http_request_t *request);
char* http_request_t___get_form_value(http_request_t* request, char* key);
#endif //UNTITLED_HTTP_REQUEST_H
