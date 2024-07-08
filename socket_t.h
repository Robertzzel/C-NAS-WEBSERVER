//
// Created by robert on 6/15/24.
//

#ifndef UNTITLED_SOCKET_T_H
#define UNTITLED_SOCKET_T_H

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "error.h"

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

error socket_t__create_context(SSL_CTX** context, const char* certificate_file, const char* private_key_file, int for_server);
error socket_t__new(socket_t* s_socket, domain domain, type type, struct ssl_ctx_st *ssl_context);
error socket_t__connect(socket_t* s_socket, const char* host, int port);
error socket_t__bind(socket_t* m_socket, const char* host, int port);
error socket_t__listen(socket_t* m_socket, int n);
error socket_t__accept(socket_t* listening_socket, socket_t* new_socket);
error socket_t__write(socket_t* s_socket, void* buffer, unsigned long buffer_size, unsigned long* bytes_written);
error socket_t__read(socket_t* s_socket, void* buffer, unsigned long buffer_size, unsigned long* bytes_read);
error socket_t__close(socket_t* m_socket);

#endif //UNTITLED_SOCKET_T_H