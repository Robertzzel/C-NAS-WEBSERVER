//
// Created by robert on 7/10/24.
//
#include "routes.h"

#define LOGIN_USERNAME_LABEL "username"
#define LOGIN_PASSWORD_LABEL "password"



bool handle_login_route_get(request_t * req, reader_t* conn) {
    response_t response;
    response_new(&response);
    response_set_status(&response, 200);
    response_add_header(&response, "Content-Type", "text/html; charset=UTF-8");
    response_add_header(&response, "Connection", "close");
    response_add_header(&response, "Access-Control-Allow-Origin", "*");
    response.body = get_login_page();

    char* string = response_to_bytes(&response);

    bytes_t b = {.size = strlen(string), .data = string};
    reader_write(conn, &b);

    response_free(&response);
    free(string);

    return true;
}

bool handle_login_route_post(request_t* request, reader_t* conn) {
    bytes_t* body_bytes = reader_read(conn, 128);
    if(body_bytes == NULL){
        return 0;
    }

    char* body_string = bytes_to_string(body_bytes);
    bytes_free(body_bytes);
    free(body_bytes);

    char *username= request_form_value(body_string, LOGIN_USERNAME_LABEL);
    if(username == NULL){
        free(body_string);
        return false;
    }
    char *password= request_form_value(body_string, LOGIN_PASSWORD_LABEL);
    if(password == NULL){
        free(body_string);
        free(username);
        return false;
    }
    free(body_string);

    bool user_exists = check_username_and_password(username, password);
    free(username);
    free(password);

    if(user_exists) {
        return handle_home_route_get(request, conn);
    }
    return handle_login_route_get(request, conn);
}