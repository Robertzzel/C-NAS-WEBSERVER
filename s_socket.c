//
// Created by robert on 6/15/24.
//
#include "s_socket.h"

int create_context(SSL_CTX** context, int forServer);
int configure_context(SSL_CTX *ctx);

int s_socket_create(s_socket* socket, domain domain, type type){
    socket->ssl_socket = NULL;
    socket->ssl_context = NULL;

    int err = socket_create(&socket->socket, domain, type);
    if(err != 0){
        return err;
    }
    return 0;
}

int s_socket_connect(s_socket* s_socket, const char* host, int port){
    int err = socket_connect(&s_socket->socket, host, port);
    if(err != 0){
        return err;
    }
    printf("client - Socket connected withot ssl\n");

    err = create_context(&s_socket->ssl_context, 0);
    if(err != 0){
        return err;
    }
    err = configure_context(s_socket->ssl_context);
    if(err != 0){
        return err;
    }

    s_socket->ssl_socket = SSL_new(s_socket->ssl_context);
    SSL_set_fd(s_socket->ssl_socket, s_socket->socket.socketfd);

    ERR_print_errors_fp(stderr);
    err = SSL_connect(s_socket->ssl_socket);
    if (err <= 0) {
        ERR_print_errors_fp(stderr);
        return 1;
    }
    return 0;
}

int s_socket_bind(s_socket* s_socket, const char* host, int port){
    return socket_bind(&s_socket->socket, host, port);
}

int s_socket_listen(s_socket* s_socket, int n){
    int err = create_context(&s_socket->ssl_context, 1);
    if(err != 0){
        return err;
    }
    err = configure_context(s_socket->ssl_context);
    if(err != 0){
        return err;
    }
    return socket_listen(&s_socket->socket, n);
}

int s_socket_accept(s_socket* listening_socket, s_socket* new_socket){
    new_socket->ssl_socket = NULL;
    new_socket->ssl_context = NULL;

    int err = socket_accept(&listening_socket->socket, &new_socket->socket);
    if(err != 0){
        return err;
    }
    printf("server - Socket accepted without ssl\n");

    new_socket->ssl_socket = SSL_new(listening_socket->ssl_context);
    SSL_set_fd(new_socket->ssl_socket, new_socket->socket.socketfd);

    if (SSL_accept(new_socket->ssl_socket) <= 0) {
        return 1;
    }

    printf("server - Socket accepted with ssl\n");

    return 0;
}

int s_socket_write(s_socket* s_socket, uint8_t* buffer, int buffer_size, int* bytes_written){
    int err = SSL_write(s_socket->ssl_socket, buffer, buffer_size);
    if(err > 0 && bytes_written != NULL){
        *bytes_written = err;
    } else if(err <= 0) {
        return 1;
    }
    return 0;
}

int s_socket_read(s_socket* s_socket, uint8_t* buffer, int buffer_size, int* bytes_read){
    int err = SSL_read(s_socket->ssl_socket, buffer, buffer_size);
    if(err > 0 && bytes_read != NULL){
        *bytes_read = err;
    } else if(err <= 0) {
        return 1;
    }

    return 0;
}

int s_socket_close(s_socket* s_socket){
    if(s_socket->ssl_socket != NULL){
        SSL_shutdown(s_socket->ssl_socket);
    }
    if(s_socket->ssl_socket != NULL){
        SSL_free(s_socket->ssl_socket);
    }
    if(s_socket->ssl_context != NULL){
        SSL_CTX_free(s_socket->ssl_context);
    }

    return socket_close(&s_socket->socket);
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

    return 0;
}

int configure_context(SSL_CTX *ctx)
{
    int err = SSL_CTX_use_certificate_file(ctx, "/home/robert/CLionProjects/untitled/mycert.pem", SSL_FILETYPE_PEM);
    if (err <= 0) {
        ERR_print_errors_fp(stderr);
        return 1;
    }

    err = SSL_CTX_use_PrivateKey_file(ctx, "/home/robert/CLionProjects/untitled/mykey.pem", SSL_FILETYPE_PEM);
    if (err <= 0 ) {
        ERR_print_errors_fp(stderr);
        return 1;
    }

    return 0;
}
