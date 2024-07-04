//
// Created by robert on 6/23/24.
//

#ifndef UNTITLED_HTTP_REQUEST_H
#define UNTITLED_HTTP_REQUEST_H

#include "string.h"
#include "stdlib.h"
#include "../error.h"
#include "../utils.h"

typedef struct {
    //request
    char* method;
    char* uri;
    char* version;

    //header
    array_of_strings_t header_names;
    array_of_strings_t headers_values;

    //body
    char* body;
} HttpRequest;

error parse_http_request(char* message, HttpRequest *request);
error free_http_request(HttpRequest *request);
#endif //UNTITLED_HTTP_REQUEST_H
