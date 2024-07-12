//
// Created by robert on 6/23/24.
//

#include "http_request.h"

char* http_request_t__get_method(const char *request_line);
char* http_request_t__get_uri(const char *request_line);
char* http_request_t__get_version(const char *request_line);
list_string_t* http_request_t__parse_header(const char *header_line);
http_request_t* http_request_t__new();

http_request_t* http_request_t__from_bytes(char* message) {
    http_request_t* request = http_request_t__new();
    if(request == NULL) {
        return NULL;
    }

    list_string_t *parts = string_split(message, HTTP_MESSAGE_DELIMITER);
    if(parts == NULL){
        return NULL;
    }

    char *request_line = list_strings_t__get(parts, 0);
    if(request_line == NULL){
        return NULL;
    }

    request->method = http_request_t__get_method(request_line);
    if(request->method == NULL){
        return NULL;
    }
    request->uri = http_request_t__get_uri(request_line);
    if(request->uri == NULL){
        return NULL;
    }
    request->version = http_request_t__get_version(request_line);
    if(request->method == NULL){
        return NULL;
    }

    int i;
    for(i = 1; i < parts->size; ++i) {
        char *header_line = list_strings_t__get(parts, i);
        if(header_line == NULL){
            http_request_t__free(request);
            list_strings_t__free(parts);
            return NULL;
        }

        if(strcmp(header_line, "") == 0) {
            break;
        }

        list_string_t *key_value_pair = http_request_t__parse_header(header_line);
        if(key_value_pair == NULL){
            break;
        }

        char* key = list_strings_t__get(key_value_pair, 0);
        char* value = list_strings_t__get(key_value_pair, 1);
        list_strings_t__add(request->header_names, key, strlen(key));
        list_strings_t__add(request->headers_values, value, strlen(value));

        list_strings_t__free(key_value_pair);
    }

    char* last_part = list_strings_t__get(parts, i + 1);
    if(last_part == NULL){
        list_strings_t__free(parts);
        return request;
    }
    request->body = xstrdup(last_part);

    list_strings_t__free(parts);
    return request;
}

list_string_t* http_request_t__parse_header(const char *header_line) {
    char* delimiter = strchr(header_line, ':');
    if(delimiter == NULL){
        return NULL;
    }

    long key_size = delimiter - header_line;
    unsigned long value_size = strlen(delimiter + 1);
    if(key_size < 1 || value_size < 1){
        return NULL;
    }

    char* key = xstrndup(header_line, key_size);
    char* value = xstrdup(delimiter + 1);

    char* clean_key = trim_whitespace(key);
    char* clean_value = trim_whitespace(value);
    free(key);
    free(value);

    list_string_t* result = list_strings_t__new();
    list_strings_t__add(result, clean_key, strlen(clean_key));
    list_strings_t__add(result, clean_value, strlen(clean_value));
    free(clean_value);
    free(clean_key);
    return result;
}

char* http_request_t__get_method(const char *request_line){
    char* delimiter = strchr(request_line, ' ');
    if(delimiter == NULL) {
        return NULL;
    }
    long method_size = delimiter - request_line;
    if(method_size < 1){
        return NULL;
    }

    return xstrndup(request_line, method_size);
}
char* http_request_t__get_uri(const char *request_line){
    char* method_end = strchr(request_line, ' ');
    if(method_end == NULL) {
        return NULL;
    }
    ++method_end;
    char* uri_end = strchr(method_end, ' ');
    if(uri_end == NULL) {
        return NULL;
    }
    long uri_size = uri_end - method_end;
    if(uri_size < 1){
        return NULL;
    }

    return xstrndup(method_end, uri_size);
}
char* http_request_t__get_version(const char *request_line){
    char* method_end = strchr(request_line, ' ');
    if(method_end == NULL) {
        return NULL;
    }
    ++method_end;
    char* uri_end = strchr(method_end, ' ');
    if(uri_end == NULL) {
        return NULL;
    }
    char* version_start = ++uri_end;
    char* version_end = strstr(version_start, "\r\n");
    if(version_end == NULL) {
        return NULL;
    }
    long version_size = version_end - version_start;
    if(version_size < 1){
        return NULL;
    }

    return xstrndup(version_start, version_size);
}

void http_request_t__free(http_request_t *request){
    if(request->method != NULL){
        free(request->method);
        request->method = NULL;
    }
    if(request->uri != NULL){
        free(request->uri);
        request->uri = NULL;
    }
    if(request->version != NULL){
        free(request->version);
        request->version = NULL;
    }
    if(request->body != NULL){
        free(request->body);
        request->body = NULL;
    }
    list_strings_t__free(request->header_names);
    list_strings_t__free(request->headers_values);
    free(request);
}

http_request_t* http_request_t__new() {
    http_request_t* request = xmalloc(sizeof(http_request_t));
    request->method = NULL;
    request->uri = NULL;
    request->version = NULL;
    request->header_names = list_strings_t__new();
    if(request->header_names == NULL){
        free(request);
        return NULL;
    }
    request->headers_values = list_strings_t__new();
    if(request->headers_values == NULL){
        free(request);
        return NULL;
    }
    request->body = NULL;

    return request;
}