//
// Created by robert on 6/14/24.
//

#include "m_socket.h"

int socket_create(m_socket* m_socket, domain domain, type type){
    m_socket->socketfd = socket(domain, type, 0);
    return m_socket->socketfd < 0;
}

int socket_connect(m_socket* m_socket, const char* host, int port){
    in_addr_t host_address = socket_resolve_hostname(host);
    if(host_address == INADDR_NONE){
        return 1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    memcpy(&addr.sin_addr.s_addr, &host_address, sizeof(in_addr_t));

    if (connect(m_socket->socketfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        return 1;
    }

    inet_ntop(AF_INET, &(addr.sin_addr), m_socket->host, INET_ADDRSTRLEN);
    return 0;
}

int socket_bind(m_socket* m_socket, const char* host, int port){
    in_addr_t host_address = socket_resolve_hostname(host);
    if(host_address == INADDR_NONE){
        return 1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    memcpy(&addr.sin_addr.s_addr, &host_address, sizeof(in_addr_t));

    if (bind(m_socket->socketfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        return 1;
    }

    return 0;
}

int socket_listen(m_socket* m_socket, int n){
    return listen(m_socket->socketfd, n);
}

int socket_accept(m_socket* listening_socket, m_socket* new_socket){
    if(new_socket == NULL){
        return 1;
    }

    socklen_t socket_address_length;
    struct sockaddr_in client_addr;
    new_socket->socketfd = accept(listening_socket->socketfd, (struct sockaddr*)&client_addr, &socket_address_length);

    inet_ntop(AF_INET, &(client_addr.sin_addr), new_socket->host , INET_ADDRSTRLEN);
    return 0;
}

int socket_write(m_socket* m_socket, uint8_t* buffer, uint64_t buffer_size, uint64_t* written){
    int w = write(m_socket->socketfd, buffer, buffer_size);
    if(written != NULL){
        *written = w;
    }
    return 0;
}

int socket_read(m_socket* m_socket, uint8_t* buffer, uint64_t buffer_size, uint64_t* bytes_read){
    int r = read(m_socket->socketfd, buffer, buffer_size);
    if(bytes_read != NULL){
        *bytes_read = r;
    }
    return 0;
}

int socket_close(m_socket* m_socket) {
    close(m_socket->socketfd);
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
