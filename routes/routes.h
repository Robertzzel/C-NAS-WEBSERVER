//
// Created by robert on 7/5/24.
//

#ifndef UNTITLED_ROUTES_H
#define UNTITLED_ROUTES_H

#include "../error.h"
#include "../http/http_request.h"
#include "../http/http_response.h"
#include "../html/html_files.h"
#include "../utils/file_utils.h"
#include "../users/users.h"
#include "stdbool.h"
#define STATIC_URL_PREFIX "/static/"
#define STATIC_DIRECTORY "/home/robert/CLionProjects/untitled/static/"

bool handle_root_route(http_request_t* request, socket_t* conn);
bool handle_not_found_route(http_request_t* request, socket_t* conn);
bool handle_login_route(http_request_t* request, socket_t* conn);
bool handle_download_route(http_request_t* request, socket_t* conn);
bool static_file_route(http_request_t *request, socket_t *conn);

#endif //UNTITLED_ROUTES_H