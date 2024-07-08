//
// Created by robert on 6/23/24.
//

#ifndef UNTITLED_HTTP_REQUEST_H
#define UNTITLED_HTTP_REQUEST_H

#include "string.h"
#include "stdlib.h"
#include "../error.h"
#include "../utils.h"
#define HTTP_MESSAGE_DELIMITER "\r\n"

typedef struct {
    char* method;
    char* uri;
    char* version;
    list_strings_t header_names;
    list_strings_t headers_values;
    char* body;
} http_request_t;

error http_request_t__from_bytes(char* message, http_request_t *request);
error http_request_t__free(http_request_t *request);
#endif //UNTITLED_HTTP_REQUEST_H