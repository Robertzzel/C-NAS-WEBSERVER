//
// Created by robert on 6/23/24.
//

#include "http_request.h"

int find_substring_index(const char *str, const char *substr);
char *trim_whitespace(char *str);
int parse_http_header(char *line, HttpRequest *request, int header_index);
int parse_http_request_line(char *line, HttpRequest *request);

int parse_http_request(char *message, HttpRequest *request) {
    request->header_count = 0;
    char line_delimiter[] = "\r\n";

    char buffer[2048];
    int next_delimiter_index = find_substring_index(message, line_delimiter);
    memcpy(buffer, message, next_delimiter_index);
    buffer[next_delimiter_index] = '\0';
    parse_http_request_line(buffer, request);

    int header_index = 0;
    char* next_header_start = message + next_delimiter_index + strlen(line_delimiter);
    while(1){
        next_delimiter_index = find_substring_index(next_header_start, line_delimiter);
        if(next_delimiter_index <= 0){
            break;
        }
        memcpy(buffer, next_header_start, next_delimiter_index);
        buffer[next_delimiter_index] = '\0';
        int err = parse_http_header(buffer, request, header_index);
        if(err != 0){
            break;
        }
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

    return 0;
}

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