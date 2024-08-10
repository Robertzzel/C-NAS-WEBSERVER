//
// Created by robert on 7/10/24.
//

#include "routes.h"

bool handle_not_found_route_get(request_t* request, socket_t* conn) {
    http_response_t response;
    http_response_t__new(&response);
    http_response_t__set_status(&response, 404);
    http_response_t__add_header(&response, "Connection", "close");
    char* string = http_response_t__to_bytes(&response);

    socket__write(conn, string, strlen(string));
    free(string);
    http_response_t__free(&response);
    return true;
}