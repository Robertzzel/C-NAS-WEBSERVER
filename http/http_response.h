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
#include "../IO/socket_t.h"

typedef struct {
    char* version;
    int status;
    char* body;
    char*** headers;
} response_t;

void response_new(response_t*);
void response_free(response_t* response);
void response_set_version(response_t* response, char* version);
bool response_set_status(response_t* response, int status);
void response_set_body(response_t* response, char* body);
void response_add_header(response_t* response, char* name, char* value);
char* response_to_bytes(response_t* response);


#endif //UNTITLED_HTTP_RESPONSE_H
