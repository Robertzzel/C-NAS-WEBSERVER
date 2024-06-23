#include "s_socket.h"
#include <pthread.h>

#define REQUEST_MAX_HEADERS 25
#define BUFFER_SIZE 2048
#define REQUEST_METHOD_MAX_SIZE 10
#define REQUEST_URI_MAX_SIZE 100
#define REQUEST_VERSION_MAX_SIZE 20
#define REQUEST_BODY_MAX_SIZE 500
#define REQUEST_HEADER_MAX_SIZE 125
#define REQUEST_HEADER_LINE_MAX_SIZE 125
#define REQUEST_REQUEST_LINE_MAX_SIZE 256

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

int find_substring_index(const char *str, const char *substr) {
    char *pos = strstr(str, substr);
    if (pos == NULL) {
        return -1;
    }
    return pos - str;
}

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

int parse_http_header(char *line, HttpRequest *request, int header_index) {
    int request_line_size = strlen(line);
    if(request_line_size >= REQUEST_HEADER_LINE_MAX_SIZE){
        return 1;
    }
    char buffer[REQUEST_HEADER_LINE_MAX_SIZE];
    memcpy(buffer, line, request_line_size + 1);
    char* strtok_r_context = buffer;

    char* header_name = strtok_r(buffer, ":", &strtok_r_context);
    if(header_name == NULL){
        return 1;
    }
    header_name = trim_whitespace(header_name);
    int header_name_size = strlen(header_name);
    if(header_name_size < REQUEST_HEADER_MAX_SIZE){
        memcpy(request->headers[header_index].data, header_name, header_name_size + 1);
        request->headers[header_index].name_start_index = 0;
    }

    int value_start_index = header_name_size + 1;
    char* header_value = strtok_r(NULL, "\r\n", &strtok_r_context);
    if(header_value == NULL){
        return 1;
    }
    header_value = trim_whitespace(header_value);
    int header_value_size = strlen(header_value);
    if(value_start_index + header_value_size < REQUEST_HEADER_MAX_SIZE){
        memcpy(&request->headers[header_index].data[value_start_index], header_value, header_value_size + 1);
        request->headers[header_index].value_start_index = value_start_index;
    }

    return 0;
}

int parse_http_request_line(char *line, HttpRequest *request) {
    int request_line_size = strlen(line);
    if(request_line_size >= REQUEST_REQUEST_LINE_MAX_SIZE){
        return 1;
    }
    char buffer[REQUEST_REQUEST_LINE_MAX_SIZE];
    memcpy(buffer, line, request_line_size + 1);

    char* strtok_r_context = buffer;

    char *method = strtok_r(buffer, " ", &strtok_r_context);
    int method_size = strlen(method);
    if(method_size < REQUEST_METHOD_MAX_SIZE) {
        memcpy(request->method, method, method_size + 1);
    }

    char *uri = strtok_r(NULL, " ", &strtok_r_context);
    int uri_size = strlen(uri);
    if(uri_size < REQUEST_URI_MAX_SIZE) {
        memcpy(request->uri, uri, uri_size + 1);
    }

    char *version = strtok_r(NULL, "\r\n", &strtok_r_context);
    int version_size = strlen(version);
    if(version_size < REQUEST_VERSION_MAX_SIZE) {
        memcpy(request->version, version, version_size + 1);
    }

    return 0;
}

// Function to parse the HTTP request message
int parse_http_request(char *message, HttpRequest *request) {
    request->header_count = 0;
    char line_delimiter[] = "\r\n";

    int next_delimiter_index = find_substring_index(message, line_delimiter);
    char* request_line = malloc(sizeof(char) * (next_delimiter_index + 1));
    if(request_line == NULL){
        return 1;
    }
    memcpy(request_line, message, next_delimiter_index);
    request_line[next_delimiter_index] = '\0';
    parse_http_request_line(request_line, request);
    free(request_line);

    int header_index = 0;
    char* next_header_start = message + next_delimiter_index + strlen(line_delimiter);
    while(1){
        next_delimiter_index = find_substring_index(next_header_start, line_delimiter);
        if(next_delimiter_index <= 0){
            break;
        }
        char* header = malloc(sizeof(char) * (next_delimiter_index + 1));
        if(header == NULL){
            return 1;
        }
        memcpy(header, next_header_start, next_delimiter_index);
        header[next_delimiter_index] = '\0';
        int err = parse_http_header(header, request, header_index);
        if(err != 0){
            free(header);
            break;
        }
        free(header);
        header_index++;
        next_header_start = next_header_start + next_delimiter_index + strlen(line_delimiter);
    }
    request->header_count = header_index;

    char* body_start = next_header_start + strlen(line_delimiter);
    int body_length = strlen(body_start);
    if(body_length <= 0) {
        return 0;
    }
    if(body_length >= REQUEST_BODY_MAX_SIZE) {
        return 1;
    }
    memcpy(request->body, body_start, body_length);
    request->body[body_length] = '\0';
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