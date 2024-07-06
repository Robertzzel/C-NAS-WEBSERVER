//
// Created by robert on 7/5/24.
//

#ifndef UNTITLED_ROUTES_H
#define UNTITLED_ROUTES_H

#include "../error.h"
#include "../http/http_request.h"
#include "../http/http_response.h"
#include "../html/html_files.h"
#include "../file_utils/file_utils.h"

error handle_root_route(http_request_t* request, s_socket* conn);
error handle_not_found_route(http_request_t* request, s_socket* conn);
error handle_download_route(http_request_t* request, s_socket* conn);

#endif //UNTITLED_ROUTES_H