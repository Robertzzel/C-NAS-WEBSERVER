//
// Created by robert on 7/10/24.
//

#include "routes.h"

bool handle_root_route(http_request_t* request, socket_t* conn) {
    return handle_login_route(request, conn);
}