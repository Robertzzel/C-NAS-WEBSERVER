#include "socket_t.h"
#include "http/http_request.h"
#include "html/html_files.h"
#include "routes/routes.h"
#define BUFFER_SIZE 1024
#include "stdio.h"
error handle_client(http_request_t* request, socket_t* conn);

int main(int argc, char *argv[]) {
    if(argc != 5) {
        printf("ex: ./binary server_host server_port certificate_file_path private_key_file_path");
        return 1;
    }

    const char* server_host = argv[1];

    char* error_converting_port = NULL;
    const long server_port = strtol(argv[2], &error_converting_port, 10);
    if(*error_converting_port){
        return 1;
    }

    char* certificate_file_path = argv[3];
    char* private_key_file_path = argv[4];

    struct ssl_ctx_st *context;
    int err = socket_t__create_context(&context, certificate_file_path, private_key_file_path, 1);
    if(err != 0){
        printf("Cannot create context\n");
        return 1;
    }

    socket_t s;
    err = socket_t__new(&s, IPv4, STREAM, context);
    if(err != 0){
        printf("Cannot create socket\n");
        return 1;
    }

    err = socket_t__bind(&s, server_host, server_port);
    if(err != 0){
        printf("Cannot bnd socket\n");
        return 1;
    }

    err = socket_t__listen(&s, 1);
    if(err != 0){
        printf("Cannot listen socket\n");
        return 1;
    }

    char buffer[BUFFER_SIZE];
    while(1) {
        socket_t client;
        err = socket_t__accept(&s, &client);
        if(err != 0){
            printf("Cannot accept sockt\n");
            break;
        }

        unsigned long read;
        err = socket_t__read(&client, buffer, BUFFER_SIZE, &read);
        if(err != 0){
            printf("Cannot read socket\n");
            break;
        }
        buffer[read] = 0;

        http_request_t request;
        err = http_request_t__from_bytes(buffer, &request);
        if(err != 0) {
            break;
        }

        handle_client(&request, &client);

        socket_t__close(&client);
        http_request_t__free(&request);
    }

    socket_t__close(&s);
    SSL_CTX_free(context);

    return 0;
}

error handle_client(http_request_t* request, socket_t* conn) {
    if(strcmp(request->uri, "/") == 0) {
        return handle_root_route(request, conn);
    } else if(strcmp(request->uri, "/download") == 0) {
        return handle_download_route(request, conn);
    }

    return handle_not_found_route(request, conn);
}