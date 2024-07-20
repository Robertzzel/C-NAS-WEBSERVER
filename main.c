#include "socket_t.h"
#include "http/http_request.h"
#include "html/html_files.h"
#include "routes/routes.h"
#define BUFFER_SIZE 2048

#include "stdio.h"

bool handle_client(http_request_t* request, socket_t* conn);

char* root_directory_path = NULL;

int main(int argc, char *argv[]) {
    if(argc != 6) {
        printf("ex: ./binary server_host server_port certificate_file_path private_key_file_path root_file_directory");
        return 1;
    }

    root_directory_path = argv[5];
    const char* server_host = argv[1];

    char* error_converting_port = NULL;
    const long server_port = strtol(argv[2], &error_converting_port, 10);
    if(*error_converting_port){
        return 1;
    }

    char* certificate_file_path = argv[3];
    char* private_key_file_path = argv[4];

    struct ssl_ctx_st *context = socket_t__create_context(certificate_file_path, private_key_file_path, 1);
    if(context == NULL){
        printf("Cannot create context\n");
        return 1;
    }

    socket_t *s = socket_t__new(IPv4, STREAM, context);
    if(s == 0){
        printf("Cannot create socket\n");
        return 1;
    }

    bool succeeded = socket_t__bind(s, server_host, server_port);
    if(!succeeded){
        printf("Cannot bnd socket\n");
        return 1;
    }

    succeeded = socket_t__listen(s, 1);
    if(!succeeded){
        printf("Cannot listen socket\n");
        return 1;
    }

    char buffer[BUFFER_SIZE];
    while(1) {
        socket_t* client = socket_t__accept(s);
        if(client == NULL){
            printf("Cannot accept sockt\n");
            continue;
        }

        int read = socket_t__read(client, buffer, BUFFER_SIZE);
        if(read < 0){
            printf("Cannot read socket\n");
            break;
        }
        buffer[read] = 0;

        http_request_t* request = http_request_t__from_bytes(buffer);
        if(request == NULL) {
            continue;
        }

        handle_client(request, client);
        socket_t__close(client);
        http_request_t__free(request);
    }

    socket_t__close(s);
    SSL_CTX_free(context);

    return 0;
}

bool handle_client(http_request_t* request, socket_t* conn) {
    if(strncmp(request->uri, STATIC_URL_PREFIX, strlen(STATIC_URL_PREFIX)) == 0){
        return static_file_route(request, conn);
    }

    if(strcmp(request->uri, "/") == 0) {
        if(strcmp(request->method, "GET") == 0) {
            return handle_root_route_get(request, conn);
        }
    }

    if(strcmp(request->uri, "/login") == 0) {
        if(strcmp(request->method, "GET") == 0) {
            return handle_login_route_get(request, conn);
        }
        if(strcmp(request->method, "POST") == 0) {
            return handle_login_route_post(request, conn);
        }
    }

    // LOGIN REQUIRED TO CONTINUE



    if(strcmp(request->uri, "/download") == 0) {
        if(strcmp(request->method, "POST") == 0) {
            return handle_download_route_post(request, conn);
        }
    }

    if(strncmp(request->uri, "/home/", strlen("/home/")) == 0) {
        if(strcmp(request->method, "GET") == 0) {
            return handle_home_route_get(request, conn);
        }
    }

    return handle_not_found_route_get(request, conn);
}