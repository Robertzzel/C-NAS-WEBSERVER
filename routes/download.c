//
// Created by robert on 7/10/24.
//
#include "routes.h"


bool handle_download_route(http_request_t* request, socket_t* conn) {
    http_response_t* response = http_response_t__new();
    http_response_t__set_status(response, 200);
    http_response_t__add_header(response, "Connection", "close");
    http_response_t__add_header(response, "Content-Type", "application/octet-stream");
    http_response_t__add_header(response, "Content-Disposition", "attachment; filename=\"hello\"");
    char* string = http_response_t__to_bytes(response);
    if(string == NULL) {
        http_response_t__free(response);
        return false;
    }

    socket_t__write(conn, string, strlen(string) - 2);

    list_string_t* files = list_strings_t__new();

    //char name1[] = "/home/robert/Downloads/kali-linux-2024.1-virtualbox-amd64.7z";
    char name2[] = "/home/robert/Downloads/Sisteme de Prelucrare Grafica.rar";
    //list_strings_t__add(&files, name1, strlen(name1));
    list_strings_t__add(files, name2, strlen(name2));
    bool success = write_zip_to_socket(files, conn);
    if(!success){
        return false;
    }

    list_strings_t__free(files);
    free(string);
    http_response_t__free(response);

    return true;
}