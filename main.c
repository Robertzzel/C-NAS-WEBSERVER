#include "s_socket.h"
#include <pthread.h>

#define REQUEST_MAX_HEADERS 25
#define BUFFER_SIZE 2048
#define REQUEST_METHOD_MAX_SIZE 10
#define REQUEST_URI_MAX_SIZE 100
#define REQUEST_VERSION_MAX_SIZE 20
#define REQUEST_BODY_MAX_SIZE 500
#define REQUEST_HEADER_MAX_SIZE 125

typedef struct {
    char data[REQUEST_HEADER_MAX_SIZE];
    unsigned short name_start_index;
    unsigned short value_start_index;
} HttpHeader;

typedef struct {
    //request
    char method[REQUEST_METHOD_MAX_SIZE];
    char uri[REQUEST_URI_MAX_SIZE];
    char version[REQUEST_VERSION_MAX_SIZE];

    //header
    HttpHeader headers[REQUEST_MAX_HEADERS];
    int header_count;

    char body[REQUEST_BODY_MAX_SIZE];
    int body_size;
} HttpRequest;

char *trim_whitespace(char *str) {
    char *end;

    // Trim leading space
    while (*str == ' ') str++;

    if (*str == 0)  // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && *end == ' ') end--;

    // Write new null terminator
    *(end + 1) = 0;

    return str;
}

int parse_http_header(char *line, HttpRequest *request, int header_index, char** strtok_r_context) {
    char* header_name = strtok_r(line, ":", strtok_r_context);
    header_name = trim_whitespace(header_name);
    int header_name_size = strlen(header_name);
    if(header_name_size < REQUEST_HEADER_MAX_SIZE){
        memcpy(request->headers[header_index].data, header_name, header_name_size);
        request->headers[header_index].data[header_name_size] = '\0';
        request->headers[header_index].name_start_index = 0;
    }

    int value_start_index = header_name_size + 1;
    char* header_value = strtok_r(NULL, "\r\n", strtok_r_context);
    header_value = trim_whitespace(header_value);
    int header_value_size = strlen(header_value);
    if(value_start_index + header_value_size < REQUEST_HEADER_MAX_SIZE){
        memcpy(&request->headers[header_index].data[value_start_index], header_value, header_value_size);
        request->headers[header_index].data[value_start_index + header_value_size] = '\0';
        request->headers[header_index].value_start_index = value_start_index;
    }

    *strtok_r_context = *strtok_r_context + 1; // jump over \n

    return 0;
}

int parse_http_request_line(char *line, HttpRequest *request, char** strtok_r_context) {
    char *method = strtok_r(line, " ", strtok_r_context);
    int method_size = strlen(method);
    if(method_size < REQUEST_METHOD_MAX_SIZE) {
        memcpy(request->method, method, method_size);
        request->method[method_size] = '\0';
    }

    char *uri = strtok_r(NULL, " ", strtok_r_context);
    int uri_size = strlen(uri);
    if(uri_size < REQUEST_URI_MAX_SIZE) {
        memcpy(request->uri, uri, uri_size);
        request->uri[uri_size] = '\0';
    }

    char *version = strtok_r(NULL, "\r\n", strtok_r_context);
    int version_size = strlen(version);
    if(version_size < REQUEST_VERSION_MAX_SIZE) {
        memcpy(request->version, version, version_size);
        request->version[version_size] = '\0';
    }

    *strtok_r_context = *strtok_r_context + 1; // jump over \n
    return 0;
}

// Function to parse the HTTP request message
int parse_http_request(char *message, HttpRequest *request) {
    request->header_count = 0;

    char* strtok_r_context = message;

    parse_http_request_line(message, request, &strtok_r_context);

    char *header = strtok_r(NULL, "\r\n", &strtok_r_context);
    int empty_line = strcmp(header, "");
    int header_index = 0;
    while(empty_line != 0){
        parse_http_header(header, request, header_index, &strtok_r_context);

        header = strtok_r(NULL, "\r\n", &strtok_r_context);
        if(header == NULL) {
            break;
        }
        empty_line = strcmp(header, "");
        header_index++;
    }

    request->header_count = header_index;

    // TODO: PARSE BODY
    return 0;
}

int create_context(SSL_CTX** context, const char* certificate_file, const char* private_key_file, int for_server);

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
        buffer[read] = '\0';

        HttpRequest request;
        parse_http_request(buffer, &request);

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