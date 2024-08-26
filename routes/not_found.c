//
// Created by robert on 7/10/24.
//

#include "routes.h"

bool handle_not_found_route_get(request_t* request, reader_t* conn) {
    response_t response;
    response_new(&response);
    response_set_status(&response, 404);
    response_add_header(&response, "Connection", "close");
    char* string = response_to_bytes(&response);

    bytes_t b = {.data = string, .size = strlen(string)};
    reader_write(conn, &b);

    free(string);
    response_free(&response);
    return true;
}