#include "routes.h"
extern char* root_directory_path;

bool handle_home_route_get(request_t* request, reader_t* conn) {
    char* uri_file_name = request->uri + strlen("/home/");
    char* uri_file_name_end = strchr(request->uri, '?');
    char* file_name = uri_file_name_end == NULL? xstrdup(uri_file_name) : xstrndup(uri_file_name, uri_file_name_end - uri_file_name);
    if(!check_path(file_name)) {
        free(file_name);
        return false;
    }

    char* full_file_path = string__concatenate_strings(3, root_directory_path, "/", file_name);
    free(file_name);

    list_file_t* l = list_directory(full_file_path);
    if(l == NULL){
        return false;
    }
    free(full_file_path);

    response_t response;
    response_new(&response);
    response_set_status(&response, 200);
    response_add_header(&response, "Content-Type", "text/html; charset=UTF-8");
    response_add_header(&response, "Connection", "close");
    response_add_header(&response, "Access-Control-Allow-Origin", "*");

    char* body = get_home_page(l);
    list_file_t__free(l);
    response_set_body(&response, body);
    free(body);

    char* response_msg = response_to_bytes(&response);
    bytes_t b = {.data = response_msg, .size = strlen(response_msg)};
    reader_write(conn, &b);
    free(response_msg);

    return true;
}

bool handle_home_route_post(request_t* request, reader_t* conn) {

    return true;
}