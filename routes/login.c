//
// Created by robert on 7/10/24.
//
#include "routes.h"

bool handle_login_route_get(http_request_t* request, socket_t* conn) {
    http_response_t* response = http_response_t__new();
    if(response == NULL) {
        return false;
    }
    http_response_t__set_status(response, 200);
    http_response_t__add_header(response, "Content-Type", "text/html; charset=UTF-8");
    http_response_t__add_header(response, "Connection", "close");
    http_response_t__add_header(response, "Access-Control-Allow-Origin", "*");
    response->body = get_login_page();

    char* string = http_response_t__to_bytes(response);
    if(string == NULL) {
        return false;
    }

    socket_t__write(conn, string, strlen(string));
    http_response_t__free(response);
    free(string);

    return true;
}

bool handle_login_route_post(http_request_t* request, socket_t* conn) {
    bool user_exists = check_username_and_password("Robert", "123456");
    if(!user_exists){
        return handle_login_route_get(request, conn);
    }

    http_response_t* response = http_response_t__new();
    http_response_t__set_status(response, 200);
    http_response_t__add_header(response, "Content-Type", "text/html; charset=UTF-8");
    http_response_t__add_header(response, "Connection", "close");
    http_response_t__add_header(response, "Access-Control-Allow-Origin", "*");
    list_string_t* form_parts = string_split(request->body, "&");
    if(form_parts == NULL) {
        http_response_t__free(response);
        return false;
    }

    char *username, *password;
    for(int i = 0; i < form_parts->size; ++i){
        char* part = list_strings_t__get(form_parts, i);
        list_string_t* key_value_pair = string_split(part, "=");;
        if(key_value_pair == NULL){
            http_response_t__free(response);
            return false;
        }

        char *key = list_strings_t__get(key_value_pair, 0);
        char *value = list_strings_t__get(key_value_pair, 1);

        list_strings_t__free(key_value_pair);
    }
    list_strings_t__free(form_parts);


//
//    list_strings_t__free(&form_parts);
//    err = http_response_t__set_body(&response, string);
//    if(err != SUCCESS) {
//        return err;
//    }
//    err = http_response_t__to_bytes(&response, &string);
//    if(err != SUCCESS) {
//        return err;
//    }
//
//    socket_t__write(conn, string, strlen(string), NULL);
    http_response_t__free(response);
    //free(string);
    return true;
}

bool handle_login_route(http_request_t* request, socket_t* conn) {
    if(strcmp(request->method, "GET") == 0){
        return handle_login_route_get(request, conn);
    } else if(strcmp(request->method, "POST") == 0) {
        return handle_login_route_post(request, conn);
    }
    return false;
}