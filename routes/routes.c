//
// Created by robert on 7/5/24.
//

#include "routes.h"
#include "zlib.h"

error handle_root_route(http_request_t* request, socket_t* conn) {
    return handle_login_route(request, conn);
}

error handle_login_route(http_request_t* request, socket_t* conn) {
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
    err = get_login_page(&response.body);
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

    //char name1[] = "/home/robert/Downloads/kali-linux-2024.1-virtualbox-amd64.7z";
    char name2[] = "/home/robert/Downloads/Sisteme de Prelucrare Grafica.rar";
    //list_strings_t__add(&files, name1, strlen(name1));
    list_strings_t__add(&files, name2, strlen(name2));
    err = write_zip_to_socket(&files, conn);
    if(err != SUCCESS){
        return err;
    }

    list_strings_t__free(&files);
    free(string);
    http_response_t__free(&response);

    return SUCCESS;
}

error static_file_route(http_request_t *request, socket_t *conn) {
    error err = check_path(request->uri);
    if(err != SUCCESS){
        return err;
    }

    char* filename = strstr(request->uri, STATIC_URL_PREFIX) + strlen(STATIC_URL_PREFIX);
    char* full_file_path = calloc(sizeof(char), strlen(STATIC_DIRECTORY) + strlen(filename) + 1);
    if(full_file_path == NULL) {
        return FAIL;
    }
    strcat(full_file_path, STATIC_DIRECTORY);
    strcat(full_file_path, filename);

    char* file_type = find_char_from_end(full_file_path, '.');
    if(file_type == NULL){
        return FAIL;
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
        return FAIL;
    }

    FILE *f = fopen(full_file_path, open_mode);
    if(f == NULL){
        return FAIL;
    }

    http_response_t response;
    err = http_response_t__new(&response);
    if(err != SUCCESS){
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
    err = http_response_t__add_header(&response, "Content-Type", content_type);
    if(err != SUCCESS) {
        return err;
    }

    char* response_string;
    err = http_response_t__to_bytes(&response, &response_string);
    if(err != SUCCESS) {
        return err;
    }

    socket_t__write(conn, response_string, strlen(response_string) - 2, NULL);

    char buffer[1024];
    size_t bytes_read;
    while((bytes_read = fread(buffer, 1, 1024, f)) > 0){
        socket_t__write(conn, buffer, bytes_read, NULL);
    }

    socket_t__write(conn, "\r\n", 2, NULL);

    free(response_string);
    free(full_file_path);
    http_response_t__free(&response);
    fclose(f);

    return SUCCESS;
}