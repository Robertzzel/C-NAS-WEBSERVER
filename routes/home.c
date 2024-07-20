#include "routes.h"
extern char* root_directory_path;

bool handle_home_route_get(http_request_t* request, socket_t* conn) {
    char* path = string__copy(request->uri + 6);
    if(!check_path(path)) {
        free(path);
        return false;
    }

    char* full_file_path = string__concatenate_strings(3, root_directory_path, "/", path);
    free(path);

    list_file_t* l = list_directory(full_file_path);
    if(l == NULL){
        return false;
    }
    free(full_file_path);

    http_response_t response;
    http_response_t__new(&response);
    http_response_t__set_status(&response, 200);
    http_response_t__add_header(&response, "Content-Type", "text/html; charset=UTF-8");
    http_response_t__add_header(&response, "Connection", "close");
    http_response_t__add_header(&response, "Access-Control-Allow-Origin", "*");

    char* body = get_home_page(l);
    list_file_t__free(l);
    http_response_t__set_body(&response, body);
    free(body);

    char* response_msg = http_response_t__to_bytes(&response);
    socket_t__write(conn, response_msg, strlen(response_msg));
    free(response_msg);

    return true;
}

bool handle_home_route_post(http_request_t* request, socket_t* conn) {

    return true;
}