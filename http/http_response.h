//
// Created by robert on 7/3/24.
//

#ifndef UNTITLED_HTTP_RESPONSE_H
#define UNTITLED_HTTP_RESPONSE_H

#include "stddef.h"
#include "../error.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "../utils.h"
#include "../socket_t.h"

typedef struct {
    char* version;
    int status;
    list_strings_t header_names;
    list_strings_t header_values;
    char* body;
} http_response_t;

error http_response_t__new(http_response_t* response);
error http_response_t__free(http_response_t* response);
error http_response_t__set_version(http_response_t* response, char* version);
error http_response_t__set_status(http_response_t* response, int status);
error http_response_t__set_body(http_response_t* response, char* body);
error http_response_t__add_header(http_response_t* response, char* name, char* value);
error http_response_t__to_bytes(http_response_t* response, char** string);
error http_response_t__write_to_socket(http_response_t* response, socket_t* socket, int write_body);


#endif //UNTITLED_HTTP_RESPONSE_H
