#include "routes.h"

bool handle_home_route_get(http_request_t* request, socket_t* conn) {
    char* path = xstrdup(request->uri + 6);
    bool valid_path = check_path(path);
    if(!valid_path) {
        free(path);
        return false;
    }

    list_file_t* l = list_file_t__new();
    file_t f;
    f.name = "HEHE";
    for(int i=0;i<15;i++){
        list_file_t__insert(l, &f);
    }


    http_response_t * response = http_response_t__new();
    http_response_t__set_status(response, 200);
    http_response_t__add_header(response, "Content-Type", "text/html; charset=UTF-8");
    http_response_t__add_header(response, "Connection", "close");
    http_response_t__add_header(response, "Access-Control-Allow-Origin", "*");
    char* body = get_home_page(l);
    http_response_t__set_body(response, body);
    char* response_msg = http_response_t__to_bytes(response);
    socket_t__write(conn, response_msg, strlen(response_msg));

    free(body);
    free(response_msg);
    free(path);
    list_file_t__free(l);
    return true;
}

bool handle_home_route_post(http_request_t* request, socket_t* conn) {

    return true;
}