//
// Created by robert on 6/15/24.
//

#ifndef UNTITLED_SOCKET_T_H
#define UNTITLED_SOCKET_T_H

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <stdio.h>
#include <stdlib.h>
#include "stdbool.h"
#include "../utils/utils.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

typedef enum {
    IPv4 = AF_INET
} domain;

typedef enum {
    STREAM = SOCK_STREAM,
    DGRAM = SOCK_DGRAM
} type;

typedef struct {
    int socketfd;
    SSL_CTX *ssl_context;
    SSL *ssl_socket;
} socket_t;

SSL_CTX* socket__create_context(const char* certificate_file, const char* private_key_file, int for_server);
socket_t* socket__new(domain domain, type type, struct ssl_ctx_st *ssl_context);
int socket__connect(socket_t* s_socket, const char* host, int port);
int socket__bind(socket_t* m_socket, const char* host, int port);
int socket__listen(socket_t* m_socket, int n);
socket_t* socket__accept(socket_t* listening_socket);
int socket__write(socket_t* s_socket, const void* buffer, unsigned long buffer_size);
int socket__read(socket_t* s_socket, void* buffer, unsigned long buffer_size);
void socket__close(socket_t* m_socket);

#endif //UNTITLED_SOCKET_T_H
