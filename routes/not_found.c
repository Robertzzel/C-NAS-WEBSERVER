//
// Created by robert on 7/10/24.
//

#include "routes.h"

bool handle_not_found_route(http_request_t* request, socket_t* conn) {
    http_response_t* response = http_response_t__new();
    if(response == NULL) {
        return false;
    }
    http_response_t__set_status(response, 404);
    bool succeeded = http_response_t__add_header(response, "Connection", "close");
    if(!succeeded) {
        http_response_t__free(response);
        return false;
    }

    char* string = http_response_t__to_bytes(response);
    if(string == NULL) {
        http_response_t__free(response);
        return false;
    }

    socket_t__write(conn, string, strlen(string));
    free(string);
    http_response_t__free(response);
    return true;
}