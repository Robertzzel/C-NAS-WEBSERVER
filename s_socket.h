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

int s_socket_create(s_socket* socket, domain domain, type type);
int s_socket_connect(s_socket* s_socket, const char* host, int port);
int s_socket_bind(s_socket* m_socket, const char* host, int port);
int s_socket_listen(s_socket* m_socket, int n);
int s_socket_accept(s_socket* listening_socket, s_socket* new_socket);
int s_socket_write(s_socket* s_socket, uint8_t* buffer, int buffer_size, int* bytes_written);
int s_socket_read(s_socket* s_socket, uint8_t* buffer, int buffer_size, int* bytes_read);
int s_socket_close(s_socket* m_socket);

#endif //UNTITLED_S_SOCKET_H
