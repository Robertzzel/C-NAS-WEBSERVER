//
// Created by robert on 6/23/24.
//

#ifndef UNTITLED_HTTP_REQUEST_H
#define UNTITLED_HTTP_REQUEST_H

#include "string.h"
#include "stdlib.h"

#define REQUEST_MAX_HEADERS 25
#define REQUEST_METHOD_MAX_SIZE 10
#define REQUEST_URI_MAX_SIZE 100
#define REQUEST_VERSION_MAX_SIZE 20
#define REQUEST_BODY_MAX_SIZE 500
#define REQUEST_HEADER_MAX_SIZE 125
#define REQUEST_HEADER_LINE_MAX_SIZE 125
#define REQUEST_REQUEST_LINE_MAX_SIZE 256

typedef struct {
    char data[REQUEST_HEADER_MAX_SIZE];
    unsigned short name_start_index;
    unsigned short value_start_index;
} HttpHeader;

typedef struct {
    //request
    char method[REQUEST_METHOD_MAX_SIZE];
    char uri[REQUEST_URI_MAX_SIZE];
    char version[REQUEST_VERSION_MAX_SIZE];

    //header
    HttpHeader headers[REQUEST_MAX_HEADERS];
    int header_count;

    char body[REQUEST_BODY_MAX_SIZE];
    int body_size;
} HttpRequest;

int parse_http_request(char *message, HttpRequest *request);
#endif //UNTITLED_HTTP_REQUEST_H
