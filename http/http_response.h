//
// Created by robert on 7/3/24.
//

#ifndef UNTITLED_HTTP_RESPONSE_H
#define UNTITLED_HTTP_RESPONSE_H

#include "stddef.h"
#include "../error.h"
#include "stdlib.h"
#include "stdio.h"
#include "../utils/utils.h"
#include "../socket_t.h"

typedef struct {
    char* version;
    int status;
    list_string_t *header_names;
    list_string_t *header_values;
    char* body;
} http_response_t;

void http_response_t__new(http_response_t*);
void http_response_t__free(http_response_t* response);
void http_response_t__set_version(http_response_t* response, char* version);
bool http_response_t__set_status(http_response_t* response, int status);
void http_response_t__set_body(http_response_t* response, char* body);
void http_response_t__add_header(http_response_t* response, char* name, char* value);
char* http_response_t__to_bytes(http_response_t* response);


#endif //UNTITLED_HTTP_RESPONSE_H
