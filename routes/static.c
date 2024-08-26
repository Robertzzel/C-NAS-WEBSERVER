//
// Created by robert on 7/10/24.
//

#include "routes.h"

bool static_file_route(request_t *request, reader_t* conn) {
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

    response_t response;
    response_new(&response);

    response_set_status(&response, 200);
    response_add_header(&response, "Connection", "close");
    response_add_header(&response, "Content-Type", content_type);

    char* response_string = response_to_bytes(&response);
    if(response_string == NULL) {
        response_free(&response);
        return false;
    }

    bytes_t b = {.size = strlen(response_string) - 2, .data = response_string};
    reader_write(conn, &b);

    char buffer[1024];
    size_t bytes_read;
    while((bytes_read = fread(buffer, 1, 1024, f)) > 0){
        b.size = bytes_read;
        b.data = buffer;
        reader_write(conn, &b);
    }

    b.size = 2;
    b.data = "\r\n";
    reader_write(conn, &b);

    free(response_string);
    response_free(&response);
    fclose(f);

    return true;
}