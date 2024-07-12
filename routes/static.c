//
// Created by robert on 7/10/24.
//

#include "routes.h"

bool static_file_route(http_request_t *request, socket_t *conn) {
    bool success = check_path(request->uri);
    if(!success){
        return false;
    }

    char* filename = strstr(request->uri, STATIC_URL_PREFIX) + strlen(STATIC_URL_PREFIX);
    char* full_file_path = xmalloc(sizeof(char) * (strlen(STATIC_DIRECTORY) + strlen(filename) + 1));
    memset(full_file_path, 0, sizeof(char) * (strlen(STATIC_DIRECTORY) + strlen(filename) + 1));

    strcat(full_file_path, STATIC_DIRECTORY);
    strcat(full_file_path, filename);

    char* file_type = find_char_from_end(full_file_path, '.');
    if(file_type == NULL){
        free(full_file_path);
        return false;
    }

    char* open_mode;
    char* content_type;
    if(strcmp(file_type, ".css") == 0) {
        open_mode = "r";
        content_type = "text/css";
    } else if(strcmp(file_type, ".js") == 0){
        open_mode = "r";
        content_type = "application/javascript";
    } else if(strcmp(file_type, ".jpg") == 0){
        content_type = "image/jpg";
        open_mode = "rb";
    } else if(strcmp(file_type, ".png") == 0){
        content_type = "image/png";
        open_mode = "rb";
    } else {
        free(full_file_path);
        return false;
    }

    FILE *f = fopen(full_file_path, open_mode);
    if(f == NULL){
        free(full_file_path);
        return false;
    }
    free(full_file_path);

    http_response_t* response = http_response_t__new();
    if(response == NULL){
        return false;
    }
    http_response_t__set_status(response, 200);
    success = http_response_t__add_header(response, "Connection", "close");
    if(!success) {
        http_response_t__free(response);
        return false;
    }
    success = http_response_t__add_header(response, "Content-Type", content_type);
    if(!success) {
        http_response_t__free(response);
        return false;
    }

    char* response_string = http_response_t__to_bytes(response);
    if(response_string == NULL) {
        http_response_t__free(response);
        return false;
    }

    socket_t__write(conn, response_string, strlen(response_string) - 2);

    char buffer[1024];
    size_t bytes_read;
    while((bytes_read = fread(buffer, 1, 1024, f)) > 0){
        socket_t__write(conn, buffer, bytes_read);
    }

    socket_t__write(conn, "\r\n", 2);

    free(response_string);
    http_response_t__free(response);
    fclose(f);

    return true;
}