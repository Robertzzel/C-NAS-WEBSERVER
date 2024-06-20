//
// Created by robert on 6/15/24.
//
#include "s_socket.h"

in_addr_t socket_resolve_hostname(const char *hostname);
int create_context(SSL_CTX** context, int forServer);
int get_address(const char* host, int port, struct sockaddr_in* addr);

int socket_create(s_socket* s_socket, domain domain, type type){
    s_socket->ssl_socket = NULL;
    s_socket->ssl_context = NULL;

    s_socket->socketfd = socket(domain, type, 0);
    if(s_socket->socketfd == -1) {
        return 1;
    }

    setsockopt(s_socket->socketfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

    return 0;
}

int socket_connect(s_socket* s_socket, const char* host, int port){
    struct sockaddr_in addr;
    int err = get_address(host, port, &addr);
    if(err != 0){
        return 1;
    }

    err = connect(s_socket->socketfd, (struct sockaddr *)&addr, sizeof(addr));
    if (err < 0) {
        return 1;
    }

    err = create_context(&s_socket->ssl_context, 0);
    if(err != 0){
        return err;
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

int socket_bind(s_socket* m_socket, const char* host, int port){
    struct sockaddr_in addr;
    int err = get_address(host, port, &addr);
    if(err != 0){
        return 1;
    }

    if (bind(m_socket->socketfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        return 1;
    }

    return 0;
}

int socket_listen(s_socket* m_socket, int n){
    return listen(m_socket->socketfd, n);
}

int socket_accept(s_socket* listening_socket, s_socket* new_socket){
    new_socket->ssl_socket = NULL;
    new_socket->ssl_context = NULL;

    if(new_socket == NULL){
        return 1;
    }

    socklen_t socket_address_length;
    struct sockaddr_in client_addr;
    new_socket->socketfd = accept(listening_socket->socketfd, (struct sockaddr*)&client_addr, &socket_address_length);

    int err = create_context(&listening_socket->ssl_context, 1);
    if(err != 0){
        return err;
    }

    new_socket->ssl_socket = SSL_new(listening_socket->ssl_context);
    SSL_set_fd(new_socket->ssl_socket, new_socket->socketfd);

    if(listening_socket->ssl_context != NULL){
        SSL_CTX_free(listening_socket->ssl_context);
        listening_socket->ssl_context = NULL;
    }

    if (SSL_accept(new_socket->ssl_socket) <= 0) {
        return 1;
    }

    return 0;
}

int socket_write(s_socket* s_socket, uint8_t* buffer, int buffer_size, int* bytes_written){
    int err = SSL_write(s_socket->ssl_socket, buffer, buffer_size);
    if(err > 0 && bytes_written != NULL){
        *bytes_written = err;
    } else if(err <= 0) {
        return 1;
    }
    return 0;
}

int socket_read(s_socket* s_socket, uint8_t* buffer, int buffer_size, int* bytes_read){
    int err = SSL_read(s_socket->ssl_socket, buffer, buffer_size);
    if(err > 0 && bytes_read != NULL){
        *bytes_read = err;
    } else if(err <= 0) {
        return 1;
    }

    return 0;
}

int socket_close(s_socket* s_socket){
    if(s_socket->ssl_socket != NULL){
        SSL_shutdown(s_socket->ssl_socket);
        SSL_free(s_socket->ssl_socket);
        s_socket->ssl_socket = NULL;
    }
    if(s_socket->ssl_context != NULL){
        SSL_CTX_free(s_socket->ssl_context);
        s_socket->ssl_context = NULL;
    }

    close(s_socket->socketfd);
    return 0;
}

int create_context(SSL_CTX** context, int forServer)
{
    const SSL_METHOD *method;

    if(forServer == 1){
        method = TLS_server_method();
    }else{
        method = TLS_client_method();
    }
    *context = SSL_CTX_new(method);
    if (!*context) {
        return 1;
    }

    int err = SSL_CTX_use_certificate_file(*context, "/home/robert/CLionProjects/untitled/mycert.pem", SSL_FILETYPE_PEM);
    if (err <= 0) {
        ERR_print_errors_fp(stderr);
        return 1;
    }

    err = SSL_CTX_use_PrivateKey_file(*context, "/home/robert/CLionProjects/untitled/mykey.pem", SSL_FILETYPE_PEM);
    if (err <= 0 ) {
        ERR_print_errors_fp(stderr);
        return 1;
    }

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
