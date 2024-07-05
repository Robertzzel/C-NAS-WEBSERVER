//
// Created by robert on 7/5/24.
//

#include "routes.h"
#define DOWNLOAD_BUFFER_SIZE (4 * 1024)

error handle_root_route(http_request_t* request, s_socket* conn) {
    http_response_t response;
    error err = http_response_new(&response);
    if(err != SUCCESS) {
        return err;
    }
    err = http_response_set_status(&response, 200);
    if(err != SUCCESS) {
        return err;
    }
    err = http_response_add_header(&response, "Content-Type", "text/html; charset=UTF-8");
    if(err != SUCCESS) {
        return err;
    }
    err = http_response_add_header(&response, "Connection", "close");
    if(err != SUCCESS) {
        return err;
    }
    err = get_home_page(&response.body);
    if(err != SUCCESS) {
        return err;
    }
    char* string;
    err = http_response_to_bytes(&response, &string);
    if(err != SUCCESS) {
        return err;
    }

    socket_write(conn, string, strlen(string), NULL);
    http_response_free(&response);
    free(string);

    return SUCCESS;
}

error handle_not_found_route(http_request_t* request, s_socket* conn) {
    http_response_t response;
    error err = http_response_new(&response);
    if(err != SUCCESS) {
        return err;
    }
    err = http_response_set_status(&response, 404);
    if(err != SUCCESS) {
        return err;
    }
    err = http_response_add_header(&response, "Connection", "close");
    if(err != SUCCESS) {
        return err;
    }

    char* string;
    err = http_response_to_bytes(&response, &string);
    if(err != SUCCESS) {
        return err;
    }

    socket_write(conn, string, strlen(string), NULL);
    free(string);
    http_response_free(&response);
    return SUCCESS;
}

error handle_download_route(http_request_t* request, s_socket* conn) {
    http_response_t response;
    error err = http_response_new(&response);
    if(err != SUCCESS) {
        return err;
    }
    err = http_response_set_status(&response, 200);
    if(err != SUCCESS) {
        return err;
    }
    err = http_response_add_header(&response, "Connection", "close");
    if(err != SUCCESS) {
        return err;
    }
    err = http_response_add_header(&response, "Content-Type", "application/octet-stream");
    if(err != SUCCESS) {
        return err;
    }
    err = http_response_add_header(&response, "Content-Disposition", "attachment; filename=\"hello\"");
    if(err != SUCCESS) {
        return err;
    }
    char* string;
    err = http_response_to_bytes(&response, &string);
    if(err != SUCCESS) {
        return err;
    }

    socket_write(conn, string, strlen(string) - 2, NULL);
    FILE *f = fopen("/home/robert/Downloads/kali-linux-2024.1-virtualbox-amd64.7z", "r");
    if(f == NULL) {
        return FAIL;
    }
    char buffer[DOWNLOAD_BUFFER_SIZE];
    size_t bytes_read = fread(buffer, 1, DOWNLOAD_BUFFER_SIZE, f);
    while (bytes_read > 0){
        socket_write(conn, buffer, bytes_read, NULL);
        bytes_read = fread(buffer, 1, DOWNLOAD_BUFFER_SIZE, f);
    }
    socket_write(conn, "\r\n", 2, NULL);

    fclose(f);
    free(string);
    http_response_free(&response);

    return SUCCESS;
}