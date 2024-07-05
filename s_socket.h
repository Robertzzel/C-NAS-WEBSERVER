//
// Created by robert on 6/15/24.
//

#ifndef UNTITLED_S_SOCKET_H
#define UNTITLED_S_SOCKET_H

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
} s_socket;

int socket_create_context(SSL_CTX** context, const char* certificate_file, const char* private_key_file, int for_server);
int socket_create(s_socket* s_socket, domain domain, type type, struct ssl_ctx_st *ssl_context);
int socket_connect(s_socket* s_socket, const char* host, int port);
int socket_bind(s_socket* m_socket, const char* host, int port);
int socket_listen(s_socket* m_socket, int n);
int socket_accept(s_socket* listening_socket, s_socket* new_socket);
int socket_write(s_socket* s_socket, char* buffer, unsigned long buffer_size, unsigned long* bytes_written);
int socket_read(s_socket* s_socket, char* buffer, unsigned long buffer_size, unsigned long* bytes_read);
int socket_close(s_socket* m_socket);

#endif //UNTITLED_S_SOCKET_H
