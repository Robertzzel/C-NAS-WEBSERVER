//
// Created by robert on 6/14/24.
//

#ifndef UNTITLED_M_SOCKET_H
#define UNTITLED_M_SOCKET_H

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
   char host[INET_ADDRSTRLEN];
} m_socket;

int socket_create(m_socket* socket, domain domain, type type);
int socket_connect(m_socket* m_socket, const char* host, int port);
int socket_bind(m_socket* m_socket, const char* host, int port);
int socket_listen(m_socket* m_socket, int n);
int socket_accept(m_socket* listening_socket, m_socket* new_socket);
int socket_write(m_socket* m_socket, uint8_t* buffer, uint64_t buffer_size, uint64_t* written);
int socket_read(m_socket* m_socket, uint8_t* buffer, uint64_t buffer_size, uint64_t* bytes_read);
int socket_close(m_socket* m_socket);
in_addr_t socket_resolve_hostname(const char *hostname);

#endif //UNTITLED_M_SOCKET_H
