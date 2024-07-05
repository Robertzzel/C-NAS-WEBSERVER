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
#include "../s_socket.h"

typedef struct {
    char* version;
    int status;
    array_of_strings_t header_names;
    array_of_strings_t header_values;
    char* body;
} http_response_t;

error http_response_new(http_response_t* response);
error http_response_free(http_response_t* response);
error http_response_set_version(http_response_t* response, char* version);
error http_response_set_status(http_response_t* response, int status);
error http_response_set_body(http_response_t* response, char* body);
error http_response_add_header(http_response_t* response, char* name, char* value);
error http_response_to_bytes(http_response_t* response, char** string);
error http_response_write_to_socket(http_response_t* response, s_socket* socket, int write_body);


#endif //UNTITLED_HTTP_RESPONSE_H
