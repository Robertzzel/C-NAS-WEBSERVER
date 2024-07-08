//
// Created by robert on 7/5/24.
//

#include "routes.h"
#include "zlib.h"

error handle_root_route(http_request_t* request, socket_t* conn) {
    http_response_t response;
    error err = http_response_t__new(&response);
    if(err != SUCCESS) {
        return err;
    }
    err = http_response_t__set_status(&response, 200);
    if(err != SUCCESS) {
        return err;
    }
    err = http_response_t__add_header(&response, "Content-Type", "text/html; charset=UTF-8");
    if(err != SUCCESS) {
        return err;
    }
    err = http_response_t__add_header(&response, "Connection", "close");
    if(err != SUCCESS) {
        return err;
    }
    err = get_home_page(&response.body);
    if(err != SUCCESS) {
        return err;
    }
    char* string;
    err = http_response_t__to_bytes(&response, &string);
    if(err != SUCCESS) {
        return err;
    }

    socket_t__write(conn, string, strlen(string), NULL);
    http_response_t__free(&response);
    free(string);

    return SUCCESS;
}

error handle_not_found_route(http_request_t* request, socket_t* conn) {
    http_response_t response;
    error err = http_response_t__new(&response);
    if(err != SUCCESS) {
        return err;
    }
    err = http_response_t__set_status(&response, 404);
    if(err != SUCCESS) {
        return err;
    }
    err = http_response_t__add_header(&response, "Connection", "close");
    if(err != SUCCESS) {
        return err;
    }

    char* string;
    err = http_response_t__to_bytes(&response, &string);
    if(err != SUCCESS) {
        return err;
    }

    socket_t__write(conn, string, strlen(string), NULL);
    free(string);
    http_response_t__free(&response);
    return SUCCESS;
}

error handle_download_route(http_request_t* request, socket_t* conn) {
    http_response_t response;
    error err = http_response_t__new(&response);
    if(err != SUCCESS) {
        return err;
    }
    err = http_response_t__set_status(&response, 200);
    if(err != SUCCESS) {
        return err;
    }
    err = http_response_t__add_header(&response, "Connection", "close");
    if(err != SUCCESS) {
        return err;
    }
    err = http_response_t__add_header(&response, "Content-Type", "application/octet-stream");
    if(err != SUCCESS) {
        return err;
    }
    err = http_response_t__add_header(&response, "Content-Disposition", "attachment; filename=\"hello\"");
    if(err != SUCCESS) {
        return err;
    }
    char* string;
    err = http_response_t__to_bytes(&response, &string);
    if(err != SUCCESS) {
        return err;
    }

    socket_t__write(conn, string, strlen(string) - 2, NULL);

    list_strings_t files;
    list_strings_t__new(&files);

    char name1[] = "/home/robert/Downloads/kali-linux-2024.1-virtualbox-amd64.7z";
    char name2[] = "/home/robert/Downloads/Sisteme de Prelucrare Grafica.rar";
    list_strings_t__add(&files, name1, strlen(name1));
    list_strings_t__add(&files, name2, strlen(name2));
    err = write_zip_to_socket(&files, conn);
    if(err != SUCCESS){
        return err;
    }
    list_strings_t__free(&files);

    //socket_t__write(conn, "\r\n", 2, NULL);
    free(string);
    http_response_t__free(&response);

    return SUCCESS;
}