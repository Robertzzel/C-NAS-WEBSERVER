//
// Created by robert on 7/10/24.
//
#include "routes.h"

#define LOGIN_USERNAME_LABEL "username"
#define LOGIN_PASSWORD_LABEL "password"



bool handle_login_route_get(request_t* request, socket_t* conn) {
    http_response_t response;
    http_response_t__new(&response);
    http_response_t__set_status(&response, 200);
    http_response_t__add_header(&response, "Content-Type", "text/html; charset=UTF-8");
    http_response_t__add_header(&response, "Connection", "close");
    http_response_t__add_header(&response, "Access-Control-Allow-Origin", "*");
    response.body = get_login_page();

    char* string = http_response_t__to_bytes(&response);
    socket__write(conn, string, strlen(string));

    http_response_t__free(&response);
    free(string);

    return true;
}

bool handle_login_route_post(request_t* request, socket_t* conn) {
    char *username= http_request_t___get_form_value(request, LOGIN_USERNAME_LABEL);
    if(username == NULL){
        return false;
    }
    char *password= http_request_t___get_form_value(request, LOGIN_PASSWORD_LABEL);
    if(password == NULL){
        free(username);
        return false;
    }

    bool user_exists = check_username_and_password(username, password);
    free(username);
    free(password);

    if(user_exists) {
        return handle_home_route_get(request, conn);
    }
    return handle_login_route_get(request, conn);
}