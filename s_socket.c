//
// Created by robert on 6/15/24.
//
#include "s_socket.h"

in_addr_t socket_resolve_hostname(const char *hostname);
int get_address(const char* host, int port, struct sockaddr_in* addr);

int socket_create(s_socket* s_socket, domain domain, type type, struct ssl_ctx_st *ssl_context){
    if(s_socket == NULL || ssl_context == NULL) {
        return 1;
    }

    s_socket->ssl_socket = NULL;
    s_socket->ssl_context = ssl_context;

    s_socket->socketfd = socket(domain, type, 0);
    if(s_socket->socketfd == -1) {
        return 1;
    }

    setsockopt(s_socket->socketfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

    return 0;
}

int socket_connect(s_socket* s_socket, const char* host, int port){
    if(s_socket == NULL || host == NULL) {
        return 1;
    }

    struct sockaddr_in addr;
    int err = get_address(host, port, &addr);
    if(err != 0){
        return 1;
    }

    err = connect(s_socket->socketfd, (struct sockaddr *)&addr, sizeof(addr));
    if (err < 0) {
        return 1;
    }

    s_socket->ssl_socket = SSL_new(s_socket->ssl_context);
    SSL_set_fd(s_socket->ssl_socket, s_socket->socketfd);

    err = SSL_connect(s_socket->ssl_socket);
    if (err <= 0) {
        ERR_print_errors_fp(stderr);
        return 1;
    }
    return 0;
}

int socket_bind(s_socket* s_socket, const char* host, int port){
    if(s_socket == NULL || host == NULL) {
        return 1;
    }

    struct sockaddr_in addr;
    int err = get_address(host, port, &addr);
    if(err != 0){
        return 1;
    }

    if (bind(s_socket->socketfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        return 1;
    }

    return 0;
}

int socket_listen(s_socket* s_socket, int n){
    if(s_socket == NULL) {
        return 1;
    }
    return listen(s_socket->socketfd, n);
}

int socket_accept(s_socket* listening_socket, s_socket* new_socket){
    if(listening_socket == NULL || new_socket == NULL){
        return 1;
    }

    new_socket->ssl_socket = NULL;
    new_socket->ssl_context = NULL;

    socklen_t socket_address_length;
    struct sockaddr_in client_addr;
    new_socket->socketfd = accept(listening_socket->socketfd, (struct sockaddr*)&client_addr, &socket_address_length);
    new_socket->ssl_socket = SSL_new(listening_socket->ssl_context);
    SSL_set_fd(new_socket->ssl_socket, new_socket->socketfd);

    if (SSL_accept(new_socket->ssl_socket) <= 0) {
        ERR_print_errors_fp(stderr);
        return 1;
    }

    return 0;
}

int socket_write(s_socket* s_socket, uint8_t* buffer, int buffer_size, int* bytes_written){
    if(s_socket == NULL || buffer == NULL){
        return 1;
    }

    int err = SSL_write(s_socket->ssl_socket, buffer, buffer_size);
    if(err > 0 && bytes_written != NULL){
        *bytes_written = err;
    } else if(err <= 0) {
        return 1;
    }
    return 0;
}

int socket_read(s_socket* s_socket, uint8_t* buffer, int buffer_size, int* bytes_read){
    if(s_socket == NULL || buffer == NULL){
        return 1;
    }

    int err = SSL_read(s_socket->ssl_socket, buffer, buffer_size);
    if(err > 0 && bytes_read != NULL){
        *bytes_read = err;
    } else if(err <= 0) {
        return 1;
    }

    return 0;
}

int socket_close(s_socket* s_socket){
    if(s_socket == NULL) {
        return 1;
    }

    if(s_socket->ssl_socket != NULL){
        SSL_shutdown(s_socket->ssl_socket);
        SSL_free(s_socket->ssl_socket);
        s_socket->ssl_socket = NULL;
    }

    close(s_socket->socketfd);
    return 0;
}

int get_address(const char* host, int port, struct sockaddr_in* addr) {
    in_addr_t host_address = socket_resolve_hostname(host);
    if(host_address == INADDR_NONE){
        return 1;
    }

    memset(addr, 0, sizeof(addr));
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    memcpy(&addr->sin_addr.s_addr, &host_address, sizeof(in_addr_t));
    return 0;
}

in_addr_t socket_resolve_hostname(const char *hostname) {
    struct addrinfo hints, *res, *p;
    struct sockaddr_in *ipv4;
    in_addr_t addr = INADDR_NONE;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(hostname, NULL, &hints, &res) != 0) {
        return INADDR_NONE;
    }

    for (p = res; p != NULL; p = p->ai_next) {
        ipv4 = (struct sockaddr_in *)p->ai_addr;
        addr = ipv4->sin_addr.s_addr;
        break;
    }

    freeaddrinfo(res);
    return addr;
}
