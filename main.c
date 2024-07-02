#include "s_socket.h"
#include <pthread.h>
#include "http_request.h"

#define BUFFER_SIZE 2048


int create_context(SSL_CTX** context, const char* certificate_file, const char* private_key_file, int for_server);
int handle_client(HttpRequest* request, s_socket* conn);

int main(int argc, char *argv[]) {
    if(argc != 5) {
        printf("ex: ./binary server_host server_port certificate_file_path private_key_file_path");
        return 1;
    }

    const char* server_host = argv[1];

    char* error_converting_port = NULL;
    const long server_port = strtol(argv[2], &error_converting_port, 10);
    if(*error_converting_port){
        printf("port not avalabile\n");
        return 1;
    }

    char* certificate_file_path = argv[3];
    char* private_key_file_path = argv[4];

    struct ssl_ctx_st *context;
    int err = create_context(&context, certificate_file_path, private_key_file_path, 1);
    if(err != 0){
        printf("server: cannot create ssl context");
        return 1;
    }

    s_socket s;
    err = socket_create(&s, IPv4, STREAM, context);
    if(err != 0){
        printf("server: cannot create socket");
        return 1;
    }
    printf("server - Socket created\n");

    err = socket_bind(&s, server_host, server_port);
    if(err != 0){
        printf("server: cannot bind socket");
        return 1;
    }
    printf("server - Socket binded\n");

    err = socket_listen(&s, 1);
    if(err != 0){
        printf("server: cannot listen socket");
        return 1;
    }
    printf("server - Socket listening\n");

    char buffer[BUFFER_SIZE];
    while(1) {
        s_socket client;
        err = socket_accept(&s, &client);
        if(err != 0){
            printf("server: cannot accept socket");
            break;
        }
        printf("server - Socket accepted\n");

        int read;
        err = socket_read(&client, (uint8_t *) buffer, BUFFER_SIZE, &read);
        if(err != 0){
            printf("server: cannot write socket");
            break;
        }
        buffer[read] = 0;

        HttpRequest request;
        err = parse_http_request(buffer, &request);
        if(err != 0) {
            break;
        }
        handle_client(&request, &client);

        free_http_request(&request);
        socket_close(&client);
    }

    socket_close(&s);
    SSL_CTX_free(context);

    return 0;
}

int create_context(SSL_CTX** context, const char* certificate_file, const char* private_key_file, int for_server)
{
    const SSL_METHOD *method;

    if(for_server == 1){
        method = TLS_server_method();
    }else{
        method = TLS_client_method();
    }
    *context = SSL_CTX_new(method);
    if (!*context) {
        return 1;
    }

    int err = SSL_CTX_use_certificate_file(*context, certificate_file, SSL_FILETYPE_PEM);
    if (err <= 0) {
        ERR_print_errors_fp(stderr);
        return 1;
    }

    err = SSL_CTX_use_PrivateKey_file(*context, private_key_file, SSL_FILETYPE_PEM);
    if (err <= 0 ) {
        ERR_print_errors_fp(stderr);
        return 1;
    }

    return 0;
}

int handle_client(HttpRequest* request, s_socket* conn) {
    printf("REQUEST: %s\n", request->uri);
    int isRoot = strcmp(request->uri, "/") == 0;
    if (isRoot){
        char buffer[] = "THIS IS ROOT\0";
        socket_write(conn, buffer, strlen(buffer), NULL);
    }

    return 0;
}