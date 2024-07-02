//
// Created by robert on 6/23/24.
//

#ifndef UNTITLED_HTTP_REQUEST_H
#define UNTITLED_HTTP_REQUEST_H

#include "string.h"
#include "stdlib.h"
#include "m_string.h"

typedef struct {
    //request
    char* method;
    char* uri;
    char* version;

    //header
    int header_count;
    char** header_names;
    char** headers_values;

    //body
    char* body;
} HttpRequest;

error parse_http_request(char* message, HttpRequest *request);
error free_http_request(HttpRequest *request);
#endif //UNTITLED_HTTP_REQUEST_H
