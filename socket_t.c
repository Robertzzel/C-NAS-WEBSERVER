//
// Created by robert on 6/15/24.
//
#include "socket_t.h"
#include "utils/utils.h"

in_addr_t socket_resolve_hostname(const char *hostname);
int get_address(const char* host, int port, struct sockaddr_in* addr);

socket_t* socket_t__new(domain domain, type type, struct ssl_ctx_st *ssl_context){
    if(ssl_context == NULL) {
        return NULL;
    }

    int socketfd = socket(domain, type, 0);
    if(socketfd == -1) {
        return NULL;
    }

    socket_t *s_socket = xmalloc(sizeof(socket_t ));
    s_socket->ssl_socket = NULL;
    s_socket->ssl_context = ssl_context;
    s_socket->socketfd = socketfd;

    setsockopt(s_socket->socketfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

    return s_socket;
}

bool socket_t__connect(socket_t* s_socket, const char* host, int port){
    if(s_socket == NULL || host == NULL) {
        return false;
    }

    struct sockaddr_in addr;
    int err = get_address(host, port, &addr);
    if(err != 0){
        return false;
    }

    err = connect(s_socket->socketfd, (struct sockaddr *)&addr, sizeof(addr));
    if (err < 0) {
        return false;
    }

    s_socket->ssl_socket = SSL_new(s_socket->ssl_context);
    SSL_set_fd(s_socket->ssl_socket, s_socket->socketfd);

    err = SSL_connect(s_socket->ssl_socket);
    if (err <= 0) {
        ERR_print_errors_fp(stderr);
        return false;
    }
    return true;
}

bool socket_t__bind(socket_t* m_socket, const char* host, int port){
    if(m_socket == NULL || host == NULL) {
        return false;
    }

    struct sockaddr_in addr;
    int err = get_address(host, port, &addr);
    if(err != 0){
        return err;
    }

    if (bind(m_socket->socketfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        return false;
    }

    return true;
}

bool socket_t__listen(socket_t* m_socket, int n){
    if(listen(m_socket->socketfd, n) == -1){
        return false;
    }
    return true;
}

socket_t* socket_t__accept(socket_t* listening_socket){
    if(listening_socket == NULL){
        return NULL;
    }

    socklen_t socket_address_length;
    struct sockaddr_in client_addr;
    int socketfd = accept(listening_socket->socketfd, (struct sockaddr*)&client_addr, &socket_address_length);
    struct ssl_st* ssl_socket = SSL_new(listening_socket->ssl_context);
    SSL_set_fd(ssl_socket, socketfd);

    if (SSL_accept(ssl_socket) <= 0) {
        SSL_free(ssl_socket);
        ERR_print_errors_fp(stderr);
        return NULL;
    }

    socket_t *new_socket = xmalloc(sizeof(socket_t));
    new_socket->socketfd = socketfd;
    new_socket->ssl_socket = ssl_socket;

    return new_socket;
}

int socket_t__write(socket_t* s_socket, const void* buffer, unsigned long buffer_size){
    if(s_socket == NULL || buffer == NULL){
        return -1;
    }

    return SSL_write(s_socket->ssl_socket, buffer, (int)buffer_size);
}

int socket_t__read(socket_t* s_socket, void* buffer, unsigned long buffer_size){
    if(s_socket == NULL || buffer == NULL){
        return -1;
    }

    return SSL_read(s_socket->ssl_socket, buffer, (int)buffer_size);
}

void socket_t__close(socket_t* m_socket){
    if(m_socket == NULL) {
        return;
    }

    if(m_socket->ssl_socket != NULL){
        SSL_shutdown(m_socket->ssl_socket);
        SSL_free(m_socket->ssl_socket);
        m_socket->ssl_socket = NULL;
    }

    close(m_socket->socketfd);
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

SSL_CTX* socket_t__create_context(const char* certificate_file, const char* private_key_file, int for_server)
{
    const SSL_METHOD *method;

    if(for_server == 1){
        method = TLS_server_method();
    }else{
        method = TLS_client_method();
    }

    SSL_CTX* context = SSL_CTX_new(method);
    if (!context) {
        return NULL;
    }

    int err = SSL_CTX_use_certificate_file(context, certificate_file, SSL_FILETYPE_PEM);
    if (err <= 0) {
        //ERR_print_errors_fp(stderr);
        return NULL;
    }

    err = SSL_CTX_use_PrivateKey_file(context, private_key_file, SSL_FILETYPE_PEM);
    if (err <= 0 ) {
        return NULL;
    }

    return context;
}
